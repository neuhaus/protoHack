#define main restored_game_main
#include "hack.main.c"
#undef main
#include <assert.h>
extern char *killer;

static void check_record(void)
{
    FILE *fp=fopen("record","r");
    int level, count=0;
    unsigned points;
    char text[128];
    assert(fp);
    while(fscanf(fp,"%d %u %127[^\n]\n",&level,&points,text)==3) {
        assert(points==(count==0?1000u:(unsigned)(11-count)));
        if(!count) assert(!strcmp(text,"regression,quit"));
        count++;
    }
    assert(count==10);
    fclose(fp);
}

int main(void)
{
    FILE *fp=fopen("record","w");
    assert(fp);
    for(int i=0;i<10;i++) fprintf(fp,"1 %d earlier,quit\n",10-i);
    fclose(fp);
    uname="regression";
    killer="quit";
    u.urexp=1000;
    assert(atexit(check_record)==0);
    /* Previously reads rec[-1] after shifting the ten existing entries. */
    done("quit");
    return 1;
}
