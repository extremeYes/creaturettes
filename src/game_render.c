////////////////////////////////
/// game_render.c

#include "game.h"

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
