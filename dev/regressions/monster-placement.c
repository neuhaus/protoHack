#define main restored_game_main
#include "hack.main.c"
#undef main
#include <assert.h>

/* Select a valid form, isolating initial coordinates from selection bounds. */
int rand(void) { return 162; } /* row 2, column 2 */
int main(void)
{
    assert(makemon(&mon[6][6])==0);
    assert(fmon && fmon->data==&mon[2][2]);
    assert(fmon->mx==0 && fmon->my==0);
    free(fmon);
    fmon=0;
    return 0;
}
