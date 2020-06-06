#include "raylib.h"
#include <ctime>
#include <iostream>
#include "constants.hpp"
#include "map.hpp"
#include "character.hpp"
#include "map_generator.hpp"
#include "image_cache.h"
#include "gen_icons.h"
#include "game_manager.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

int main() {

    srand(time(nullptr));
    int seed = rand();
    std::cout << "seed: " << seed << '\n';
    gltactics::game_manager manager(seed);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {      // Detect window close button or ESC key
        manager.handleInput();
        manager.stepState();
        manager.renderGameState();
#ifdef __EMSCRIPTEN__
        emscripten_sleep(1000/60);
#endif
    }

    // De-Initialization
    CloseWindow();        // Close window and OpenGL context

    return 0;
}

