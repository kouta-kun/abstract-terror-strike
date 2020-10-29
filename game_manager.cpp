//
// Created by Bedroom1 on 31/05/2020.
//

#include "game_manager.h"
#include "image_cache.h"
#include <iostream>
#include <regex>

gltactics::ghost<> gltactics::game_manager::makeGhost(gltactics::map<> &map) {
    std::uniform_int_distribution<size_t> xyMap(1, DEFAULT_MAPSIZE - 2);
    std::array<size_t, 2> xyVec{};
    do {
        xyVec = {xyMap(generator), xyMap(generator)};
    } while (map[xyVec].tileType != AIR);
    return gltactics::ghost<>(RED, {(xyVec[1]), (xyVec[0])}, map, generator);
}

gltactics::game_manager::game_manager(std::mt19937_64 &generator, platform_builder builder) :
        generator{generator},
        mapGenerator(generator),
        currentMap(mapGenerator.buildMap()),
        _playerCharacter({1, 1}, currentMap),
        _ghost{makeGhost(currentMap)},
        platformModule{builder(*this)} {
    platformModule->initialize();
}

gltactics::character<> &gltactics::game_manager::getPlayerCharacter() {
    return _playerCharacter;
}

gltactics::map<> &gltactics::game_manager::getMap() {
    return currentMap;
}

void gltactics::game_manager::stepState() {
    if (moveDirection) {
        _playerCharacter.move(*moveDirection);
        if (currentMap[_playerCharacter.position()].tileType == EXIT) {
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

void gltactics::game_manager::renderGameState() {
    this->platformModule->render();
}

void gltactics::game_manager::handleInput() {
    auto in = this->platformModule->get_input();
    moveDirection = in.movementDirection;
    useEnvironment = in.environment;
    useItems = in.items;
}

const gltactics::ghost<> &gltactics::game_manager::getGhost() const {
    return _ghost;
}
