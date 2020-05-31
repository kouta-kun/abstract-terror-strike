//
// Created by Bedroom1 on 31/05/2020.
//

#include "game_manager.h"
#include "image_cache.h"
#include "rendering.hpp"

gltactics::game_manager::game_manager(int seed) : mapGenerator(seed), currentMap(mapGenerator.buildMap()),
                                                  _playerCharacter(BLUE, (Vector2) {1, 1}, currentMap),
                                                  camera{.position = (Vector3) {0.0f, 15.0f, 0.0f},
                                                          .target= (Vector3) {0.0f, 0.0f, 0.0f},
                                                          .up = (Vector3) {0.0f, 0.0f, -1.0f}, .fovy = 90.0f,
                                                          .type = CAMERA_PERSPECTIVE} {
    InitWindow(screenWidth, screenHeight, "gltactics");
    SetTargetFPS(60);                                        // Set our game to run at 60 frames-per-second
}

gltactics::character<> &gltactics::game_manager::getPlayerCharacter() {
    return _playerCharacter;
}

gltactics::map<> &gltactics::game_manager::getMap() {
    return currentMap;
}

void gltactics::game_manager::handleInput() {
    moveDirection = std::optional<gltactics::direction>();
    useItems = false;
    useEnvironment = false;
    if (IsKeyPressed(KEY_LEFT))
        moveDirection = gltactics::direction::left;
    if (IsKeyPressed(KEY_RIGHT))
        moveDirection = gltactics::direction::right;
    if (IsKeyPressed(KEY_DOWN))
        moveDirection = gltactics::direction::down;
    if (IsKeyPressed(KEY_UP))
        moveDirection = gltactics::direction::up;
    if (IsKeyPressed(KEY_E))
        useItems = true;
    else if (IsKeyPressed(KEY_F))
        useEnvironment = true;
}

void gltactics::game_manager::stepState() {
    if (moveDirection) {
        _playerCharacter.move(*moveDirection);
        if (currentMap[_playerCharacter.positionArray()].tileType == EXIT) {
            currentMap = mapGenerator.buildMap();
            _playerCharacter = currentMap;
        }
    }
    if (useItems)
        _playerCharacter.useItems();
    if (useEnvironment)
        _playerCharacter.useEnvironment();
}