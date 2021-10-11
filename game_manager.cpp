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
    return gltactics::ghost<>(RED, {.x=float(xyVec[1]), .y=float(xyVec[0])}, map, generator);
}

gltactics::game_manager::game_manager(std::mt19937_64 &generator) :
        camera{
                .position = (Vector3) {0.0f, 15.0f, 0.0f},
                .target= (Vector3) {0.0f, 0.0f, 0.0f},
                .up = (Vector3) {0.0f, 0.0f, -1.0f}, .fovy = 90.0f,
                .projection = CAMERA_PERSPECTIVE
        },
        generator{generator},
        mapGenerator(generator),
        currentMap(mapGenerator.buildMap()),
        _playerCharacter(SKYBLUE, {1, 1}, currentMap),
        _ghost{makeGhost(currentMap)} {
    InitWindow(screenWidth, screenHeight, "gltactics");
    SetTargetFPS(60); // Set our game to run at 30 frames-per-second
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
            stepGen();
        }
    }
    if (useItems)
        _playerCharacter.useItems();
    if (useEnvironment)
        _playerCharacter.useEnvironment();
    _ghost.stepAi();
}

void gltactics::game_manager::stepGen() {
    this->currentMap = this->mapGenerator.buildMap();
    this->_playerCharacter = this->currentMap;
    this->_ghost = this->currentMap;
}
