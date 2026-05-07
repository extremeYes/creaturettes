////////////////////////////////
/// main.c

#include "game.c"

int main(void) {
#ifdef RELEASE
#else
#endif // RELEASE

    int screenX = 1280;
    int screenY = 720;

    /// 2D Starts:

    InitWindow(screenX, screenY, "creaturettes");
    SetTargetFPS(FPS_COEF * 60);
    SetExitKey(KEY_BACKSPACE);

    game_init();

    while (!WindowShouldClose()) {

        game_process_inputs();

        BeginDrawing();
        ClearBackground(DARKBROWN);

        game_run_tick();
        game_draw_ui();

        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
