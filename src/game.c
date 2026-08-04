////////////////////////////////
/// game.c

#include "game_render.c"
#include "game_init.c"
#include "game_tick.c"

void game_process_inputs(void) {
    /// Menu Keys

    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_F11) || (IsKeyPressed(KEY_ENTER) && IsKeyDown(KEY_LEFT_ALT))) {
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
    if      (IsKeyPressed(KEY_ZERO )) party_mode = 0;
    else if (IsKeyPressed(KEY_ONE  )) party_mode = 1;
    else if (IsKeyPressed(KEY_TWO  )) party_mode = 2;
    else if (IsKeyPressed(KEY_THREE)) party_mode = 3;

    /// GAMEPLAY!

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // TODO: start unit selection square
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        // end unit selection square
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
            "SPACE|F11|Alt+ENTER: Play/Pause\n"
            "F5: Reset Simulation\n"
            "F8: Display Debug Info\n"
            "r: reset on-off\n"
            "f: fast forward 16 seconds (slow)\n"

            , 20, 52, 20, CLITERAL(Color){ 0, 228, 48, (128 + 64 + 32) }
        );

        DrawText(
            "¡creaturettes!\n"
            "\n\n"
            "Welcome...\n",

            1920/2 - 300, 52, 40, CLITERAL(Color){
                GetRandomValue(0, 64),
                GetRandomValue(0, 64),
                GetRandomValue(0, 64),
                GetRandomValue(191, 255),
            }
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
