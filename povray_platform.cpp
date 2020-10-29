//
// Created by kouta on 24/7/20.
//

#include "povray_platform.h"
#include <fmt/core.h>
#include <fstream>
#include <sstream>
#include <SDL2/SDL.h>

gltactics::povray_platform::povray_platform(gltactics::game_manager &gameManager) : gameManager(gameManager) {}

void gltactics::povray_platform::initialize() {
    frameN = 0;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    this->window = SDL_CreateWindow("Abstract Terror Strike - POVRAY", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    800, 600, SDL_WINDOW_SHOWN);
    this->winSurface = SDL_GetWindowSurface(window);
}

void gltactics::povray_platform::render() {
    std::ostringstream s;
    auto[y, x] = gameManager.getPlayerCharacter().position();
    s << "#include \"colors.inc\"\n"
         "  camera {\n" +
         fmt::format("    location <{}, 40, {}>\n", x, y) +
         fmt::format("    look_at <{}, 0, {}>\n", x, y) +
         "    angle 90\n"
         "  }\n"
         "\n"
         "  light_source { <500, 500, -1000> White }\n"
         "\n"
         "union {\n"
         "  box {\n"
         "    <0, -0.5, 0>,\n"
         "    <24, 0, 24>\n"
         "    pigment { Blue }\n"
         "  }\n";
    std::vector<std::array<size_t, 2>> walls;
    std::vector<std::array<size_t, 2>> chests;
    std::vector<std::array<size_t, 2>> vDoor;
    std::vector<std::array<size_t, 2>> hDoor;
    std::vector<std::array<size_t, 2>> stairs;
    for (size_t y = 0; y < gltactics::DEFAULT_MAPSIZE; y++) {
        for (size_t x = 0; x < gltactics::DEFAULT_MAPSIZE; x++) {
            gltactics::tile tile = gameManager.getMap()[{y, x}];
            switch (tile.tileType) {
                case gltactics::type::WALL:
                    walls.push_back(std::array<size_t, 2>{y, x});
                    break;
                case gltactics::type::DOOR:
                    if (!tile.isOpen()) {
                        if (tile.isHorizontal()) {
                            hDoor.push_back(std::array<size_t, 2>{y, x});
                        } else {
                            vDoor.push_back(std::array<size_t, 2>{y, x});
                        }
                    }
                    break;
                case gltactics::type::CHEST:
                    chests.push_back(std::array<size_t, 2>{y, x});
                    break;
                case gltactics::type::EXIT:
                    stairs.push_back(std::array<size_t, 2>{y, x});
                    break;
            }
        }
    }
    for (auto[y, x] : walls) {
        s << "  box {\n" <<
          fmt::format("    <{}, 0, {}>,\n", x, y) <<
          fmt::format("    <{}, 1, {}>", x + 1, y + 1) << "\n  }\n";
    }
    s << "  pigment { White }\n}\n";
    for (auto[y, x] : chests) {
        s << "  box {\n" <<
          fmt::format("    <{}, 0, {}>,\n", float(x) + 0.3, float(y) + 0.3) <<
          fmt::format("    <{}, 0.5, {}>\n", float(x) + 0.7, float(y) + 0.7) <<
          "    pigment { Yellow }\n  }\n";
    }
    for (auto[y, x] : vDoor) {
        s << "  box {\n" <<
          fmt::format("    <{}, 0, {}>,\n", float(x) + 0.4, float(y)) <<
          fmt::format("    <{}, 1, {}>\n", float(x) + 0.6, float(y) + 1) <<
          "    pigment { Yellow }\n  }\n";
    }
    for (auto[y, x] : hDoor) {
        s << "  box {\n" <<
          fmt::format("    <{}, 0, {}>,\n", float(x), float(y) + 0.4) <<
          fmt::format("    <{}, 1, {}>\n", float(x) + 1, float(y) + 0.6) <<
          "    pigment { Yellow }\n  }\n";
    }
    s << "  box {\n" <<
      fmt::format("    <{}, 0, {}>,\n", float(x), float(y)) <<
      fmt::format("    <{}, 1, {}>\n", float(x) + 1, float(y) + 1) <<
      "    pigment { Orange }\n  }\n";
    std::ofstream frame(fmt::format("/tmp/atsframe.pov", frameN++));
    frame << s.str();
    frame.close();
    system("povray /tmp/atsframe.pov +Fb -o/tmp/atsframe.bmp");
    auto frameSurf = SDL_LoadBMP("/tmp/atsframe.bmp");
    SDL_BlitSurface(frameSurf, NULL, winSurface, NULL);
    SDL_FreeSurface(frameSurf);
    SDL_UpdateWindowSurface(window);
}

gltactics::input_status gltactics::povray_platform::get_input() {
    SDL_Event e;
    input_status status = input_status();
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    status.movementDirection = gltactics::direction::up;
                    break;
                case SDLK_DOWN:
                    status.movementDirection = gltactics::direction::down;
                    break;
                case SDLK_LEFT:
                    status.movementDirection = gltactics::direction::left;
                    break;
                case SDLK_RIGHT:
                    status.movementDirection = gltactics::direction::right;
                    break;

                case SDLK_f:
                    status.environment = true;
                    break;

                case SDLK_e:
                    status.items = true;
                    break;
            }
        }
    }
    return status;
}
