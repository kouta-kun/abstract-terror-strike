//
// Created by Bedroom1 on 31/05/2020.
//

#ifndef ABSTRACT_TERROR_STRIKE_GAME_MANAGER_H
#define ABSTRACT_TERROR_STRIKE_GAME_MANAGER_H

#include "map.hpp"
#include "map_generator.hpp"
#include "character.hpp"

namespace gltactics {
    class game_manager {
        gltactics::map_generator mapGenerator;
        gltactics::map<> currentMap;
        gltactics::character<> _playerCharacter;
        Camera3D camera;
        std::optional<gltactics::direction> moveDirection;
        bool useItems;
        bool useEnvironment;
    public:
        static const size_t screenWidth = 640;
        static const size_t screenHeight = 480;
        game_manager(int seed);
        gltactics::character<> &getPlayerCharacter();
        gltactics::map<> &getMap();
        void nextMap();
        void handleInput();
        void stepState();
        void renderGameState();
        void cameraPositionChunk();
        static void drawFloor();
        void drawPlayer();
        static void renderTile(const Vector3 &tilePosition, const gltactics::tile &tile);
        void drawHud();
        [[nodiscard]] std::array<float, 2> calculateCameraChunk() const;
    };
};


#endif //ABSTRACT_TERROR_STRIKE_GAME_MANAGER_H
