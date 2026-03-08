////////////////////////////////
/// game.c

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
#define CELL_HITPOINTS   (1 * 2 * (1024 + 512))
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
    FatStruct cells[CELL_COUNT];
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

PartyMode party_mode = 1;

i32 timer = 0;
bool show_debug_info = false;
bool reset_enabled = true;
bool gogo = false;
bool show_help = false;
i64 cycle_pos = 0;

/// Functions

u8 party_color_calculate(u8 base_color, f32 bright_factor, u8 brightness, f32 rad) {
    switch (party_mode) {
    case PARTY_MODE_INIT:
        return base_color;
    case PARTY_MODE_FIRE:
        return base_color + ((250 - base_color) * brightness * bright_factor / 20); // flame!
    case PARTY_MODE_VLUE:
        return base_color + ((250 - base_color) * brightness / 20); // Vlue Vlask
    case PARTY_MODE_CLOWN:
        return base_color + ((GetRandomValue(0, (250 - base_color))) * rad); // CLOWN !!!!!!!!
    default:
        return base_color;
    }
}

bool draw_cell(FatStruct c) {

    const f32 distance_to_center = Vector2Length((Vector2){
        .x = c.pos.x - center.x,
        .y = c.pos.y - center.y,
    });
    const f32 opacity = 0.9f * (MAP_CIRCULAR_SIZE - distance_to_center) / MAP_CIRCULAR_SIZE;

    if (c.as.cell.is_nonexistent) return true;
    if (c.as.cell.is_dead) {
        DrawCircleV(c.pos, c.as.cell.rad, CLITERAL(Color){ 130, 50, 10, opacity * 64 });
    } else if (c.as.cell.is_colliding_w_cell) {
        const u8 brightness = GetRandomValue(1, (21 - c.as.cell.rad));
        DrawCircleV(c.pos, c.as.cell.rad, CLITERAL(Color){
            party_color_calculate(190, 0.2f, brightness, c.as.cell.rad),
            party_color_calculate(43, 0.6f, brightness, c.as.cell.rad),
            party_color_calculate(45, 0.0f, brightness, c.as.cell.rad),
            opacity * 255
        });
    } else {
        DrawCircleV(c.pos, c.as.cell.rad, CLITERAL(Color){ 160, 42, 40, opacity * 255 });
    }
    return true;
}

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

bool game_update() {
    for (i32 i = 0; i < CELL_COUNT; i++) {
        FatStruct *c = &world.cells[i];

        if (c->as.cell.is_nonexistent) continue; // The bigger continue!

        // Movement
        c->pos.x += c->vel.x;
        c->pos.y += c->vel.y;

        // Collision detection w/Walls
#define USE_CIRCULAR_MAP // temporary variable to switch to a circular map
#ifdef USE_CIRCULAR_MAP
        const f32 map_rad = MAP_CIRCULAR_SIZE;

        // AI
        // Vector from Center to Ball
        f32 dx = c->pos.x - center.x;
        f32 dy = c->pos.y - center.y;

        // Distance from center
        f32 dist = sqrtf((dx*dx) + (dy*dy));

        if (dist + c->as.cell.rad > map_rad) {
            // 1. Calculate Normal (Direction from Center to Ball)
            f32 nx = dx / dist;
            f32 ny = dy / dist;

            // 2. Position Correction (Clamp to edge)
            f32 touchDist = map_rad - c->as.cell.rad;
            c->pos.x = center.x + (nx * touchDist);
            c->pos.y = center.y + (ny * touchDist);

            // 3. Velocity Reflection (Bounce off the curved wall)
            f32 vDotN = (c->vel.x * nx) + (c->vel.y * ny);

            if (vDotN > 0) {
                f32 bounce = -2.0f * vDotN;
                c->vel.x += bounce * nx;
                c->vel.y += bounce * ny;
            }
        }

#else // USE_CIRCULAR_MAP
        i32 W = 1920; // fallback if needed
        i32 H = 1080;
        // Collision detection w/Walls
        if (c->pos.x - c->rad <= 0) {
            c->pos.x = c->rad;
            c->vel.x = -c->vel.x;
        }

        if (c->pos.x + c->rad >= W) {
            c->pos.x = W - c->rad;
            c->vel.x = -c->vel.x;
        }

        if (c->pos.y - c->rad <= 0) {
            c->pos.y = c->rad;
            c->vel.y = -c->vel.y;
        }

        if (c->pos.y + c->rad >= H) {
            c->pos.y = H - c->rad;
            c->vel.y = -c->vel.y;
        }
#endif // USE_CIRCULAR_MAP

        if (c->as.cell.is_dead) { // The big continue!
            c->as.cell.hit_counter += 1;
            continue;
        }

        c->as.cell.is_colliding_w_cell = false;

        // Collision detection w/Cells
        for (i32 j = 0; j < CELL_COUNT; j++) {
            FatStruct *c2 = &world.cells[j];

            if (i == j) continue; // !!!!!!!!

            if (c2->as.cell.is_nonexistent) continue; // The third big continue!

            // get distance squared
            f32 dx = c->pos.x - c2->pos.x;
            f32 dy = c->pos.y - c2->pos.y;

            f32 dist_squared = (dx * dx) + (dy * dy);

            f32 rad_sum = c->as.cell.rad + c2->as.cell.rad;
            f32 rad_sum_squared = rad_sum * rad_sum;

            // check
            if (dist_squared < rad_sum_squared) {
                c->as.cell.is_colliding_w_cell = true;
                c->as.cell.hit_counter += 1;

                if ( ! c->as.cell.is_dead) { // alive still // unnecessary check
                    if (c->as.cell.hit_counter >= CELL_HITPOINTS) {
                        c->as.cell.hit_counter = 0;
                        c->as.cell.is_dead = true;
                    }
                }

                // turned out to be the alive cell's responsibility
                // to "ripe" the dead cell when colliding
                if (c2->as.cell.is_dead) {
                    c2->as.cell.hit_counter += CELL_CORPSE_HIT_DAMAGE;
                    if (c2->as.cell.hit_counter >= CELL_CORPSE_SPAN) {
                        c2->as.cell.is_nonexistent = true;
                    }
                }

                f32 dist = sqrtf(dist_squared);
                if (dist == 0.0f) dist = 0.01f; // Avoid div by zero

                // Normal Vector (Direction of collision)
                f32 nx = dx / dist;
                f32 ny = dy / dist;

                // A. Position Correction (Stop them from overlapping)
                f32 overlap = rad_sum - dist;
                f32 push = overlap * 0.8f;

                // Push apart along normal
                c->pos.x -= nx * push;
                c->pos.y -= ny * push;
                c2->pos.x += nx * push;
                c2->pos.y += ny * push;

                // B. Velocity Reflection (Bounce)
                f32 rvx = c->vel.x - c2->vel.x;
                f32 rvy = c->vel.y - c2->vel.y;

                f32 vel_norm = (rvx * nx) + (rvy * ny);

                if (vel_norm > 0) continue;

                f32 impulse = -(1.0f + 1.0f) * vel_norm;
                impulse /= 2.0f; // Equal mass assumption

                // SOFTENER:
                // reduce the impulse to make it a soft sim
                impulse *= 0.02f;

                f32 ix = impulse * nx;
                f32 iy = impulse * ny;

                c->vel.x += ix;
                c->vel.y += iy;
                c2->vel.x -= ix;
                c2->vel.y -= iy;
            }
        }
    }

    return true;
}

bool game_render() {
    for (i32 i = 0; i < CELL_COUNT; i++) {
        draw_cell(world.cells[i]);
    }

    if (show_debug_info) {
#define MAP_CIRCLE_THICKNESS 3
        DrawRing((Vector2){
            .x = (1920/2), .y = (1080/2)
        }, MAP_CIRCULAR_SIZE, MAP_CIRCULAR_SIZE + MAP_CIRCLE_THICKNESS, 0.0f, 360.0f, 128, CLITERAL(Color){ 0, 0, 0, 255 });
    }

    return true;
}

void game_process_inputs(void) {
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_F11) || ( IsKeyPressed(KEY_ENTER) && IsKeyDown(KEY_LEFT_ALT) )) {
        gogo = !gogo;
        ToggleBorderlessWindowed();
    }

    if (IsKeyPressed(KEY_F1)) {
        show_help = !show_help;
    }
    if (gogo && IsKeyPressed(KEY_F5)) {
        game_init();
    }
    if (IsKeyPressed(KEY_F8)) {
        show_debug_info = !show_debug_info;
    }

    if (gogo && IsKeyPressed(KEY_F)) {
        for (i32 i = 0; i < 1000; i++) {
            game_update();
        }
    }

    if (IsKeyPressed(KEY_R)) {
        reset_enabled = !reset_enabled;
    }

    if (gogo) {
        if      (IsKeyPressed(KEY_ZERO )) party_mode = 0;
        else if (IsKeyPressed(KEY_ONE  )) party_mode = 1;
        else if (IsKeyPressed(KEY_TWO  )) party_mode = 2;
        else if (IsKeyPressed(KEY_THREE)) party_mode = 3;
    }
}

void game_run_tick(void) {
    timer += 1;
    if (reset_enabled && (timer > RESET_TIME)) {
        timer = 0;
        game_init();
    }

    if (gogo) {
        cycle_pos += 1;

        if ((cycle_pos % TIME_COEF) == 0) game_update(); // GAME !
        game_render();
    } else {
        DrawText("BackSpace: Exit", 450, 272, 20, CLITERAL(Color){ 230, 41, 55, 128 + 64 });
        DrawText(
            "SPACE: Play !\n"
            "==============", 450, 300, 40, GREEN
        );
    }
}

void game_draw_ui(void) {
    if (show_help) {
        // @@ for future use
        DrawText(
            //@@ of the pre-preprocessor
            "F1: Help\n"
            "BACKSPACE: Exit\n"
            "SPACE | F11 | Alt+ENTER: Play/Pause\n"
            "F5: Reset Simulation\n"
            "F8: Display Debug Info\n"
            "r: reset on-off\n"
            "f: fast forward 16 seconds (slow)\n"

            , 20, 52, 20, CLITERAL(Color){ 0, 228, 48, (128 + 64 + 32) }
        );

        DrawText(
            "¡creaturettes!\n"
            "\n"
            "\n"
            "Welcome...\n"

            , 1920/2-300, 52, 40, CLITERAL(Color){ 0, 0, 0, 255 }
        );
    } else {
        if (gogo) {
            DrawText("F1: Help", 20, 52, 20, CLITERAL(Color){ 0, 0, 0, 128 });
        } else {
            DrawText("F1: Help", 20, 52, 20, CLITERAL(Color){ 0, 0, 0, 255 });
        }
    }

    char timer_display[32] = {0};
    sprintf(timer_display, "t%3d", timer);

    if (show_debug_info) {
        DrawText(timer_display, 180, 34, 20, CLITERAL(Color){ 0, 126, 100, 255 });

        if (reset_enabled) {
            DrawText("reset_on",  180, 10, 20, CLITERAL(Color){  12, 196, 120, 255 });
        } else {
            DrawText("reset_off", 180, 10, 20, CLITERAL(Color){ 220,  50,  20, 255 });
        }
    }

    DrawText("creaturettes", 30, 30, 20, GREEN);
}
