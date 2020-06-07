//
// Created by Bedroom1 on 31/05/2020.
//

#include "game_manager.h"
#include "image_cache.h"
#include "rendering.hpp"

gltactics::ghost<> gltactics::game_manager::makeGhost(gltactics::map<> &map) {
    std::uniform_int_distribution<size_t> xyMap(1, DEFAULT_MAPSIZE - 2);
    std::array<size_t, 2> xyVec{};
    do {
        xyVec = {xyMap(generator), xyMap(generator)};
    } while (map[xyVec].tileType != AIR);
    gltactics::ghost<> newGhost(RED, {.x=float(xyVec[1]), .y=float(xyVec[0])}, map, generator);
    return newGhost;
}

gltactics::game_manager::game_manager(int seed) : camera{
        .position = (Vector3) {0.0f, 15.0f, 0.0f},
        .target= (Vector3) {0.0f, 0.0f, 0.0f},
        .up = (Vector3) {0.0f, 0.0f, -1.0f}, .fovy = 90.0f,
        .type = CAMERA_PERSPECTIVE},
                                                  generator(seed), mapGenerator(generator),
                                                  currentMap(mapGenerator.buildMap()),
                                                  _ghost(makeGhost(currentMap)),
                                                  _playerCharacter{
                                                          gltactics::character<>(BLUE,
                                                                                 (Vector2) {1, 1},
                                                                                 currentMap)
                                                  } {
    InitWindow(screenWidth, screenHeight, "gltactics");
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
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
            _ghost = currentMap;
        }
    }
    if (useItems)
        _playerCharacter.useItems();
    if (useEnvironment)
        _playerCharacter.useEnvironment();
    _ghost.stepAi();
}