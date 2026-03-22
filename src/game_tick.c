////////////////////////////////
/// game_tick.c

#include "game.h"

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
