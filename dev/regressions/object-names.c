#define main restored_game_main
#include "hack.main.c"
#undef main
#include <assert.h>

static void check_size(struct obj *obj, size_t size)
{
    unsigned char output[128];
    memset(output,0x5a,sizeof(output));
    doname(obj,(char *)output+17,size);
    for(size_t i=0;i<17;i++) assert(output[i]==0x5a);
    for(size_t i=17+size;i<sizeof(output);i++) assert(output[i]==0x5a);
    if(size) assert(memchr(output+17,0,size));
}

int main(void)
{
    char label[100], target[100];
    const char *types="\"%)[!?/=*x";
    struct obj object={0};
    const size_t sizes[]={0,1,2,20,63,83};
    memset(label,'x',99); label[99]=0;
    potcall[0]=scrcall[0]=wandcall[0]=ringcall[0]=label;
    object.quan=1;
    object.olet='=';
    uleft=&object;
    strcpy(target,"You were wearing ");
    doname(&object,&target[17],sizeof(target)-17);
    assert(target[99]==0);
    assert(!strncmp(target,"You were wearing a ring called ",31));

    /* Include one-byte outputs and prefixed buffers, both known/unknown items. */
    for(const char *type=types;*type;type++) {
        object.olet=*type;
        object.otyp=*type=='['?2:0;
        for(int known=0;known<=1;known++) {
            object.known=known;
            for(size_t i=0;i<sizeof(sizes)/sizeof(sizes[0]);i++)
                check_size(&object,sizes[i]);
        }
    }
    object.olet='='; object.otyp=0; object.known=0;
    ringcall[0]="lucky";
    doname(&object,target,sizeof(target));
    assert(!strcmp(target,"a ring called lucky  (on left hand)."));
    doname(&object,0,0);
    return 0;
}
