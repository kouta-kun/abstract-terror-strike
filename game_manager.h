//
// Created by Bedroom1 on 31/05/2020.
//

#ifndef ABSTRACT_TERROR_STRIKE_GAME_MANAGER_H
#define ABSTRACT_TERROR_STRIKE_GAME_MANAGER_H

static const char *const command_format = "([udlr]{1})?e?f?";

#include "map.hpp"
#include "map_generator.hpp"
#include "character.hpp"
#include "ghost.h"
#include "platform_module.h"
#include "direction.hpp"
#include <chrono>

namespace gltactics {
    class game_manager;
    typedef gltactics::platform_module*(*platform_builder)(game_manager&);
    class game_manager {
        gltactics::map_generator mapGenerator;
        gltactics::map<> currentMap;
        gltactics::character<> _playerCharacter;
        std::optional<direction> moveDirection;
        gltactics::platform_module *platformModule;
        std::mt19937_64 generator;
        ghost<> _ghost;
        bool useItems = false;
        bool useEnvironment = false;
    public:
        static const size_t screenWidth = 640;
        static const size_t screenHeight = 480;

        game_manager(std::mt19937_64 &generator, platform_builder builder);

        gltactics::character<> &getPlayerCharacter();

        gltactics::map<> &getMap();

        gltactics::ghost<> makeGhost(gltactics::map<> &map);

        const ghost<> &getGhost() const;

        void handleInput();

        void stepState();

        void renderGameState();

        void stepGen();
    };
};


#endif //ABSTRACT_TERROR_STRIKE_GAME_MANAGER_H
