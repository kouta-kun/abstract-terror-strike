//
// Created by Bedroom1 on 05/06/2020.
//

#include <iostream>
#include "ghost.h"
#include "direction.hpp"

template<size_t map_size>
gltactics::ghost<map_size>::ghost(Color color, std::array<size_t, 2> position, map<map_size> &map, std::mt19937_64 &generator)
        : _color{color},
          _position{position},
          _map{map}, generator{generator},
          target{position} {
}

template<size_t map_size>
void gltactics::ghost<map_size>::stepAi() {
    map<map_size> &map = _map;
    int ghostX = int(_position[1]);
    int ghostY = int(_position[0]);

    int targetX = int(target[1]);
    int targetY = int(target[0]);
    if ((ghostX == targetX && ghostY == targetY) || (framesWithTarget++ > 15)) {
        framesWithTarget = 0;
        std::array<size_t, 2> newPosition{};
        std::uniform_int_distribution<size_t> targetXDis(std::max(1, ghostX - 3),
                                                         std::min(int(map_size - 2), ghostX + 3));
        std::uniform_int_distribution<size_t> targetYDis(std::max(1, ghostY - 3),
                                                         std::min(int(map_size - 2), ghostY + 3));
        do {
            newPosition = {targetYDis(generator.get()), targetXDis(generator.get())};
        } while (map[newPosition].tileType != AIR);
        target = {(newPosition[1]), (newPosition[0])};
        targetX = int(target[1]);
        targetY = int(target[0]);
    }
    if (framesAlive++ % 10 == 0) {
        std::optional<gltactics::direction> direction;
        std::array<size_t, 2> targetAi;
        targetAi = map.inSameRoom(target, _position) ? target : map.doorInRoom(_position);
        std::vector<gltactics::direction> possibleDirections = directionsToward(targetAi);
        if (!possibleDirections.empty()) {
            std::uniform_int_distribution<int> index(0, possibleDirections.size() - 1);
            direction = possibleDirections[index(generator.get())];
        }
        if (direction)
            this->move(*direction);
    }
}

template<size_t map_size>
std::vector<gltactics::direction> gltactics::ghost<map_size>::directionsToward(const std::array<size_t, 2> &toward) const {
    std::vector<gltactics::direction> possibleDirections;
    if (toward[1] < _position[1]) possibleDirections.push_back(direction::left);
    else if (toward[1] > _position[1]) possibleDirections.push_back(direction::right);
    if (toward[0] < _position[0]) possibleDirections.push_back(direction::up);
    else if (toward[0] > _position[0]) possibleDirections.push_back(direction::down);
    std::remove_if(std::begin(possibleDirections), std::end(possibleDirections), [&](direction a) {
        map<map_size> &map = _map.get();
        size_t idx = size_t((_position[0] * map_size) + _position[1]) + a;
        tile &tile = map[idx];
        return tile.tileType == WALL;
    });
    return possibleDirections;
}

template<size_t map_size>
void gltactics::ghost<map_size>::move(direction direction) {
    size_t destination = (size_t) (_position[0] * map_size + _position[1]) + direction;
    map<map_size> &wrapper = _map;
    tile tile = wrapper[destination];
    type blockType = tile.tileType;
    if (blockType == DOOR) {
        int xOffset = bool(tile.attributeType & HORIZONTAL) ? 0 : (direction == direction::left ? -1 : 1);
        int yOffset = (!bool(tile.attributeType & HORIZONTAL) ? 0 : (direction == direction::up ? -1 : 1)) * map_size;
        auto offset = xOffset + yOffset;
        destination = destination + offset;
        tile = wrapper[destination];
        blockType = tile.tileType;
    }
    if (blockType == AIR) {
        _position[0] = int(destination / map_size);
        _position[1] = int(destination % map_size);
    }
}

template<size_t map_size>
std::array<size_t, 2> gltactics::ghost<map_size>::position() const {
    return _position;
}
template<size_t map_size>
gltactics::ghost<map_size> &gltactics::ghost<map_size>::operator=(gltactics::map<map_size> &newMap) {
    _map = newMap;
    return *this;
}


template
class gltactics::ghost<gltactics::DEFAULT_MAPSIZE>;