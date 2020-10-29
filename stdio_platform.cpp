//
// Created by kouta on 24/7/20.
//

#include "stdio_platform.h"
#include "direction.hpp"
#include <iostream>
#include <regex>

void gltactics::stdio_platform::initialize() {

}


void gltactics::stdio_platform::render() {
    for (size_t y = 0; y < DEFAULT_MAPSIZE; y++) {
        for (size_t x = 0; x < DEFAULT_MAPSIZE; x++) {
            gltactics::tile tile = manager.getMap()[{y, x}];
            if (manager.getPlayerCharacter().position() == std::array<size_t, 2>{y, x}) {
                std::cout << '@';
            } else if (manager.getGhost().position() == std::array<size_t, 2>{y, x}) {
                std::cout << 'X';
            } else {
                switch (tile.tileType) {
                    case gltactics::type::AIR:
                        std::cout << ' ';
                        break;
                    case gltactics::type::CHEST:
                        std::cout << 'M';
                        break;
                    case gltactics::type::DOOR:
                        if (tile.attributeType & gltactics::attribute::OPEN)
                            std::cout << '_';
                        else if (tile.attributeType & gltactics::attribute::LOCKED)
                            std::cout << '^';
                        else
                            std::cout << '/';
                        break;
                    case gltactics::type::EXIT:
                        std::cout << 'S';
                        break;
                    case gltactics::type::WALL:
                        std::cout << '=';
                        break;
                }
            }
        }
        std::cout << std::endl;
    }
}

gltactics::input_status gltactics::stdio_platform::get_input() {
    std::cout << command_format << ": ";
    std::regex regex(command_format, std::regex_constants::ECMAScript);
    auto moveDirection = std::optional<direction>();
    auto useItems = false;
    auto useEnvironment = false;
    std::string input;
    std::getline(std::cin, input);
    auto words_begin =
            std::sregex_iterator(input.begin(), input.end(), regex);
    if (words_begin != std::sregex_iterator()) {
        std::string movementString = words_begin->str();
        for (char i : movementString) {
            if (i == 'l')
                moveDirection = direction::left;
            if (i == 'r')
                moveDirection = direction::right;
            if (i == 'd')
                moveDirection = direction::down;
            if (i == 'u')
                moveDirection = direction::up;
            if (i == 'e')
                useItems = true;
            if (i == 'f')
                useEnvironment = true;
        }
    }
    return gltactics::input_status{.movementDirection = moveDirection, .items = useItems, .environment = useEnvironment};
}

gltactics::stdio_platform::stdio_platform(gltactics::game_manager &gm) : manager{gm} {
}
