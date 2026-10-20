#include <stdlib.h>
/* Isolate selection bounds from the independent uninitialized-coordinate bug. */
static void *fresh_alloc(size_t size) { return calloc(1,size); }
#define malloc fresh_alloc
#define main restored_game_main
#include "hack.main.c"
#undef main
#undef malloc
#include <assert.h>

static int moving, draws;
/* Select the last row/column, and force the periodic transformation once. */
int rand(void)
{
    if(moving && !draws++) return 0;
    return 167; /* %6=5, %8=7, %7=6 */
}

int main(void)
{
    assert(makemon(&mon[6][6])==0);
    assert(fmon && fmon->cham && fmon->data==&mon[7][6]);

    moving=1;
    fmon->mx=fmon->my=1;
    fmon->mstat=MFROZ; /* Return after transformation, before combat/movement. */
    assert(dochug(fmon)==0);
    assert(draws>0 && fmon->data==&mon[7][6]);
    free(fmon);
    fmon=0;
    return 0;
}
