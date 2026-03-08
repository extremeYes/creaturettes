////////////////////////////////
/// nob.c

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "../nob.h"

#define SRC_DIR "src/"

typedef char byte;

void announce_build_method(char *compiler, char *os) {
    printf("\e[1;31m Compiling: \e[1;34m%s\e[m on \e[1;32m%s\e[m:\n", compiler, os);
}

//@hoist
int main(int argc, byte **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    Cmd cmd = {0};
    #define C(...) cmd_append(&cmd, __VA_ARGS__);
    #define RUN if (!cmd_run_sync_and_reset(&cmd)) goto cmd_failed;

    // flags
    bool flag_force = false;

    if (!nob_mkdir_if_not_exists("./bin")) {
        printf("Could not make bin directory");
        return 1;
    }

    const char *program_name = shift(argv, argc);
    while (argc > 0) {
        const char *flag = shift(argv, argc);
        if ((!strcmp(flag, "f")) || (!strcmp(flag, "-f"))) {
            flag_force = true;
        } else {
            printf("Unknown flag: %s\n", flag);
            return 1;
        }
    }
#ifdef _WIN32
    const int nr = (nob_needs_rebuild1("main.exe", SRC_DIR"main.c") || nob_needs_rebuild1("main.exe", SRC_DIR"game.c"));
#else // _WIN32
    const int nr = (nob_needs_rebuild1("main",     SRC_DIR"main.c") || nob_needs_rebuild1("main",     SRC_DIR"game.c"));
#endif // _WIN32
    if (nr < 0) return 1;

    if (nr || flag_force) {

// Pick zig or gcc !
// #define ZIGCC_OVER_GCC

#ifdef ZIGCC_OVER_GCC
    #ifdef _WIN32
        announce_build_method("zig cc", "Windows");
        C("zig", "cc");
        C("-Wall", "-Wextra");
        C("-std=c99");
        C("-I./raylib-5.5_win64_msvc16/include/");
        // C("-DRELEASE");
        C("-o", "main.exe");
        C(SRC_DIR"main.c");
        C("-L./raylib-5.5_win64_msvc16/lib/");
        C("-lraylib", "-lgdi32", "-lwinmm");
        C("-lopengl32"); // not needed in gcc apparently
        C("-luser32", "-lshell32", "-lkernel32");
        RUN
    #else //Linux boi
        announce_build_method("zig cc", "Linux");
        C("zig", "cc");
        C("-Wall", "-Wextra");
        C("-std=gnu99");
        C("-Iraylib-5.5_linux_amd64/include");
        // C("-DRELEASE");
        C("-o", "main");
        C(SRC_DIR"main.c");
        C("-Lraylib-5.5_linux_amd64/lib");
        C("-lraylib", "-lm");
        RUN
    #endif

// gcc -Wall -Wextra -std=c99 \
// -I./raylib-5.5_win64_mingw-w64/include/ -DRELEASE -o main main.c \
// -L./raylib-5.5_win64_mingw-w64/lib/ -Xlinker -static -l:libraylib.a -lraylib -lraylib -lgdi32 -lwinmm -lm

#else // ZIGCC_OVER_GCC
    #ifdef _WIN32
        announce_build_method("gcc", "Windows");
        C("gcc");
        C("-Wall", "-Wextra");
        C("-std=c99");
        C("-I./raylib-5.5_win64_mingw-w64/include/");
        // C("-DRELEASE");
        C("-DEXPERIMENTAL");
        // optimizations!
        C("-O3", "-march=native", "-ffast-math", "-flto", "-fno-math-errno");
        // // tell me if you vectorized
        // C("-fopt-info-vec");
        // // tell me which loops were vectorized
        // C("-fopt-info-vec-optimized", "-fopt-info-vec-missed");
        C("-o", "main.exe");
        C(SRC_DIR"main.c");
        C("-L./raylib-5.5_win64_mingw-w64/lib/");
        C("-lraylib", "-lgdi32", "-lwinmm");
        RUN

        C("gcc");
        C("-Wall", "-Wextra");
        C("-std=c99");
        C("-I./raylib-5.5_win64_mingw-w64/include/");
        // C("-DRELEASE");
        // optimizations!
        C("-O3", "-march=native", "-ffast-math", "-flto", "-fno-math-errno");
        C("-o", "ignoreddir/main.exe");
        C(SRC_DIR"main.c");
        C("-L./raylib-5.5_win64_mingw-w64/lib/");
        C("-lraylib", "-lgdi32", "-lwinmm");
        RUN

    #else // _WIN32
        announce_build_method("gcc", "Linux");
        C("gcc");
        C("-Wall", "-Wextra");
        C("-std=gnu99");
        C("-I./raylib-5.5_linux_amd64/include/");
        // C("-DRELEASE");
        C("-o", "main");
        C(SRC_DIR"main.c");
        C("-L./raylib-5.5_linux_amd64/lib/");
        C("-l:libraylib.a", "-lm");
        RUN
    #endif // _WIN32
#endif // ZIGCC_OVER_GCC
    }

    C("./main"); RUN //, Forrest, Run!

    return 0;

cmd_failed: // joking, bro
    printf("\e[1;31m ^\e[m Last Command Failed!\n");
    return 1;
}
