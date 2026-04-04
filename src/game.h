////////////////////////////////
/// game.h

#ifndef GAME_H_
#define GAME_H_

///
//////
////////////
////////////////////////
////////////
//////
///

#include "headers_and_types.h"

/// Static Globals

#define FPS_COEF 1.5f // game fps cap will be 60 * FPS_COEF
#define TIME_COEF 1 // game update tick happens every this number of frames
                    // if both are equal, then game is 60 ticks per second
#define RESET_TIME (3 * 1024) // these ticks and auto-reset if enabled
#define TICKS_TO_FREEDOM 1 // ticks until map opens up

// #define MAP_CIRCULAR_SIZE (1920 / 2) // overridden by the f32 below for now (temporary)

// #define CELL_COUNT 1000
#define CELL_COUNT 2500
// #define CELL_COUNT 3500
// #define CELL_COUNT 4500
// #define CELL_COUNT 6000

// Stable (short-lived cells as per prototype_build_001)
// DO NOT modify/delete
#define PARTICELL_STARTING_VELOCITY 5.0f
#define CELL_HITPOINTS   (2 * 1 * (1024 + 512))
#define CELL_CORPSE_SPAN (1 * 8 * 1024)
#define CELL_CORPSE_HIT_DAMAGE (32)
// #define CELL_HITPOINTS   (4 * 8 * 1024)
// #define CELL_CORPSE_SPAN (2 * 8 * 1024)
// #define CELL_CORPSE_HIT_DAMAGE (32+16)

#ifdef EXPERIMENTAL

// experimental (?)
#define RESET_TIME (1 * 1536) // these ticks and auto-reset if enabled
#define PARTICELL_STARTING_VELOCITY 600.0f
#define CELL_HITPOINTS   (2 * 8 * 1024)
#define CELL_CORPSE_SPAN (4 * 8 * 1024)
#define CELL_CORPSE_HIT_DAMAGE (32+16)

#endif // EXPERIMENTAL

// #define SPAWN_PERIOD (10 * 60) // ambitious TODO, make it spawn more cells every so often

// f32 MAP_CIRCULAR_SIZE = 600 / 2;
f32 MAP_CIRCULAR_SIZE = 1920 / 2;

/// Structs

typedef struct {
    bool is_colliding_w_cell;
    bool is_dead;
    bool is_nonexistent;
    f32 rad;
    i32 hit_counter;
} Cell;

typedef struct {
} Bush;

typedef struct {
} Tree;

typedef struct {
} Animal;

/// Fat struct

typedef enum {
    K_INIT = 0,
    K_CELL,
    K_BUSH,
    K_TREE,
    K_ANIMAL,
} Kind;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    Kind kind;
    union {
        Cell cell;
        Bush bush;
        Tree tree;
        Animal animal;
    } as;
} FatStruct;

typedef struct {
    // 5 means we have 5 frames of visuals to play with. (at least that's the plan)
    FatStruct cells[5 * CELL_COUNT];
} World;

/// Dynamic Globals

World world = {0};

Vector2 center = {
    .x = 1920 / 2,
    .y = 1080 / 2,
};

typedef enum {
    PARTY_MODE_INIT = 0,
    PARTY_MODE_FIRE,
    PARTY_MODE_VLUE,
    PARTY_MODE_CLOWN,
} PartyMode;

PartyMode party_mode = PARTY_MODE_FIRE;

u32 timer = 0;
bool show_debug_info = false;
bool reset_enabled = true;
bool gogo = false;
bool show_help = false;
u32 cycle_pos = 0;

#endif // GAME_H_

///
//////
////////////
////////////////////////
////////////
//////
///
