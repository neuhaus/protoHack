#define main restored_game_main
#include "hack.main.c"
#undef main
#include <assert.h>

static void read_capture(FILE *fp, char *out, size_t size)
{
    rewind(fp);
    memset(out,0,size);
    assert(fread(out,1,size-1,fp)>0);
}

int main(void)
{
    char *text=malloc(64), output[512];
    FILE *capture=tmpfile();
    int saved_stdout=dup(STDOUT_FILENO), status;
    pid_t child;

    assert(text && capture && saved_stdout>=0);
    strcpy(text,"heap-backed message");
    assert(dup2(fileno(capture),STDOUT_FILENO)>=0);
    flags.topl=flags.botl=flags.dscr=0;
    pline("item: %s (%d) %.2f",text,42,3.25);
    fflush(stdout);
    read_capture(capture,output,sizeof(output));
    assert(strstr(output,"item: heap-backed message (42) 3.25"));

    /* panic used fixed int arguments, which truncate pointers on 64-bit hosts. */
    rewind(capture);
    child=fork();
    assert(child>=0);
    if(!child) panic("failure: %s (%d) %.2f",text,42,3.25);
    assert(waitpid(child,&status,0)==child);
    assert(WIFEXITED(status) && WEXITSTATUS(status)==100);
    read_capture(capture,output,sizeof(output));
    assert(strstr(output,"failure: heap-backed message (42) 3.25"));

    assert(dup2(saved_stdout,STDOUT_FILENO)>=0);
    close(saved_stdout);
    fclose(capture);
    free(text);
    return 0;
}
