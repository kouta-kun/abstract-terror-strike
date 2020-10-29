#include <ctime>
#include <iostream>
#include "constants.hpp"
#include "map.hpp"
#include "character.hpp"
#include "map_generator.hpp"
#include "image_cache.h"
#include "gen_icons.h"
#include "game_manager.h"
#include "stdio_platform.h"
//#include "povray_platform.h"
#include <glfw_platform.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

int main() {

    srand(time(nullptr));
    int seed = rand();
    std::cout << "seed: " << seed << '\n';
    std::mt19937_64 gen(seed);
    gltactics::game_manager
            manager(gen, [](gltactics::game_manager &a) {
        return (gltactics::platform_module *) (new gltactics::glfw_platform(a));
    });
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (true) {      // Detect window close button or ESC key
        manager.renderGameState();
        manager.handleInput();
        manager.stepState();
#ifdef __EMSCRIPTEN__
        emscripten_sleep(1000/60);
#endif
    }

    return 0;
}

