////////////////////////////////
/// game_init.c

#include "game.h"

bool game_init() {
    i32 W = 1920;
    i32 H = 1080;

    memset(&world, 0, sizeof(World));

    for (i32 i = 0; i < CELL_COUNT; i++) {
        FatStruct *c = &world.cells[i];

        c->pos.x = GetRandomValue(100, W - 100);
        c->pos.y = GetRandomValue(100, H - 100);

        c->vel.x = GetRandomValue(-PARTICELL_STARTING_VELOCITY, PARTICELL_STARTING_VELOCITY);
        c->vel.y = GetRandomValue(-PARTICELL_STARTING_VELOCITY, PARTICELL_STARTING_VELOCITY);

        c->as.cell.rad = GetRandomValue(1, 20);
    }

    return true;
}
