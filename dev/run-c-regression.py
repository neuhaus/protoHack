#!/usr/bin/env python3
"""Build and run a native C regression with ASan/UBSan (requires Clang and CMake).

The test includes hack.main.c with its entry point renamed, so it can exercise
real game functions and globals without driving an interactive terminal.
Builds and runtime files live in a temporary directory, leaving saves untouched.
Leak detection is disabled because the historical game retains process state;
address and undefined-behavior checks remain enabled and fatal.
"""
import json, os, pathlib, shlex, subprocess, sys, tempfile
root=pathlib.Path(__file__).resolve().parent.parent
if len(sys.argv)!=2:
    raise SystemExit("usage: python3 dev/run-c-regression.py dev/regressions/CASE.c")
case=pathlib.Path(sys.argv[1]).resolve()
with tempfile.TemporaryDirectory(prefix='proto-review-') as tmp:
    build=pathlib.Path(tmp)
    def run(cmd,**kw):
        p=subprocess.run(cmd,cwd=build,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,**kw)
        if p.returncode:
            print(p.stdout[-12000:]);raise SystemExit(p.returncode)
        return p.stdout
    run(['cmake','-G','Unix Makefiles','-S',str(root),'-B',str(build),'-DCMAKE_C_COMPILER=clang','-DCMAKE_BUILD_TYPE=Debug',
         '-DCMAKE_C_FLAGS=-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer'])
    run(['cmake','--build',str(build),'-j','4'])
    compile_db=json.loads((build/'compile_commands.json').read_text())
    main=next(x for x in compile_db if x['file'].endswith('/hack.main.c'))
    cmd=shlex.split(main['command'])
    out=cmd.index('-o'); cmd[out+1]=str(build/'regression.o')
    cmd[cmd.index(main['file'])]=str(case)
    run(cmd)
    link=shlex.split((build/'CMakeFiles/hack.dir/link.txt').read_text())
    link[link.index('CMakeFiles/hack.dir/src/root/hack.main.c.o')]=str(build/'regression.o')
    link[link.index('-o')+1]=str(build/'regression')
    run(link)
    env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='print_stacktrace=1')
    p=subprocess.run([str(build/'regression')],cwd=build/'hackdir',input='\n'*200,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,env=env,timeout=20)
    print(p.stdout[-8000:]);print('exit',p.returncode)
    raise SystemExit(0 if p.returncode==0 else 1)
