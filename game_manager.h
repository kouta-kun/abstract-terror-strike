//
// Created by Bedroom1 on 31/05/2020.
//

#ifndef ABSTRACT_TERROR_STRIKE_GAME_MANAGER_H
#define ABSTRACT_TERROR_STRIKE_GAME_MANAGER_H

#include "map.hpp"
#include "map_generator.hpp"
#include "character.hpp"
#include "ghost.h"
#include <chrono>

namespace gltactics {
    class game_manager {
        gltactics::map_generator mapGenerator;
        gltactics::map<> currentMap;
        gltactics::character<> _playerCharacter;
        Camera3D camera;
        std::optional<gltactics::direction> moveDirection;
        std::mt19937_64 generator;
        ghost<> _ghost;
        bool useItems = false;
        bool useEnvironment = false;
    public:
        static const size_t screenWidth = 640;
        static const size_t screenHeight = 480;

        game_manager(std::mt19937_64 &generator);

        gltactics::character<> &getPlayerCharacter();

        gltactics::map<> &getMap();

        gltactics::ghost<> makeGhost(gltactics::map<> &map);

        void handleInput();

        void stepState();

        void renderGameState();

        void cameraPositionChunk();

        static void drawFloor();

        void drawPlayer();

        static void renderTile(const Vector3 &tilePosition, const gltactics::tile &tile);

        void drawHud();

        [[nodiscard]] std::array<float, 2> calculateCameraChunk() const;

        void drawGhost();

        void stepGen();
    };
};


#endif //ABSTRACT_TERROR_STRIKE_GAME_MANAGER_H
