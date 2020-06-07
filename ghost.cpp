//
// Created by Bedroom1 on 05/06/2020.
//

#include <iostream>
#include "ghost.h"

template<size_t map_size>
gltactics::ghost<map_size>::ghost(Color color, Vector2 position, map<map_size> &map, std::mt19937_64 &generator)
        : _color{color},
          _position{position},
          _map{map}, generator{generator},
          target{position} {
}

template<size_t map_size>
void gltactics::ghost<map_size>::stepAi() {
    map<map_size> &map = _map;
    int ghostX = int(_position.x);
    int ghostY = int(_position.y);

    int targetX = int(target.x);
    int targetY = int(target.y);
    if (ghostX == targetX && ghostY == targetY) {
        std::array<size_t, 2> newPosition{};
        std::uniform_int_distribution<size_t> targetXDis(std::max(1, ghostX - 3),
                                                         std::min(int(map_size - 2), ghostX + 3));
        std::uniform_int_distribution<size_t> targetYDis(std::max(1, ghostY - 3),
                                                         std::min(int(map_size - 2), ghostY + 3));
        do {
            newPosition = {targetYDis(generator), targetXDis(generator)};
        } while (map[newPosition].tileType != AIR);
        target = {.x=static_cast<float>(newPosition[1]), .y=static_cast<float>(newPosition[0])};
        targetX = int(target.x);
        targetY = int(target.y);
    }
    std::cout << "TARGET: (" << targetX << "," << targetY << "), CURPOS: (" << ghostX << "," << ghostY << ")\n";
    if (framesAlive++ % 10 == 0) {
        std::optional<direction> direction;
        Vector2 targetAi;
        targetAi = map.inSameRoom(target, _position) ? target : map.doorInRoom(_position);
        std::vector<gltactics::direction> possibleDirections = directionsToward(targetAi);
        if (!possibleDirections.empty()) {
            std::uniform_int_distribution<int> index(0, possibleDirections.size() - 1);
            direction = possibleDirections[index(generator)];
        }
        if (direction)
            this->move(*direction);
    }
}

template<size_t map_size>
std::vector<gltactics::direction> gltactics::ghost<map_size>::directionsToward(const Vector2 &toward) const {
    std::vector<direction> possibleDirections;
    if (toward.x < _position.x) possibleDirections.push_back(gltactics::direction::left);
    else if (toward.x > _position.x) possibleDirections.push_back(gltactics::direction::right);
    if (toward.y < _position.y) possibleDirections.push_back(gltactics::direction::up);
    else if (toward.y > _position.y) possibleDirections.push_back(gltactics::direction::down);
    std::remove_if(std::begin(possibleDirections), std::end(possibleDirections), [&](direction a) {
        map<map_size> &map = _map.get();
        size_t idx = size_t((_position.y * map_size) + _position.x) + a;
        tile &tile = map[idx];
        return tile.tileType == WALL;
    });
    return possibleDirections;
}

template<size_t map_size>
void gltactics::ghost<map_size>::move(gltactics::direction direction) {
    size_t destination = (size_t) (_position.y * map_size + _position.x) + direction;
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
        _position.y = int(destination / map_size);
        _position.x = int(destination % map_size);
    }
}

template<size_t map_size>
Vector3 gltactics::ghost<map_size>::position3D() {
    return Vector3{_position.x, 0.0f, _position.y};
}

template<size_t map_size>
Color gltactics::ghost<map_size>::color() {
    return _color;
}

template<size_t map_size>
gltactics::ghost<map_size> &gltactics::ghost<map_size>::operator=(gltactics::map<map_size> &newMap) {
    _map = newMap;
    return *this;
}

template
class gltactics::ghost<gltactics::DEFAULT_MAPSIZE>;