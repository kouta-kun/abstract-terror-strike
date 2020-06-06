//
// Created by Bedroom1 on 19/05/2020.
//

#include "map.hpp"
#include "character.hpp"
#include <cassert>

template<size_t map_size>
gltactics::map<map_size>::map(uint8_t fillMap) {
    for (int y = 0; y < map_size; y++) {
        for (int x = 0; x < map_size; x++) {
            layout[y * map_size + x] = fillMap; // llenar el mapa con este valor uint8_t
        }
    }
}

template<size_t map_size>
gltactics::map<map_size>::map(std::ifstream inputFile) {
    char line[map_size + 3];
    for (int y = 0; y < map_size; y++) {
        inputFile.getline(line, map_size + 2); // leemos una linea de hasta 18 caracteres
        for (int x = 0; x < map_size; x++) {
            layout[y * map_size + x] = line[x] - '0'; // almacenamos el valor como esta en el txt
        }
    }
}

template<size_t map_size>
gltactics::tile &gltactics::map<map_size>::operator[](std::array<size_t, 2> index) {
    return layout[index[0] * map_size + index[1]];
}

template<size_t map_size>
gltactics::tile &gltactics::map<map_size>::operator[](size_t index) {
    return layout[index];
}

template<size_t map_size>
std::optional<gltactics::chest> gltactics::map<map_size>::getChest(size_t id) {
    if (this->chests[id] != nullptr) {
        return *(this->chests[id]);
    } else return std::optional<gltactics::chest>();
}

template<size_t map_size>
void gltactics::map<map_size>::setChest(size_t id, gltactics::chest *ptr) {
    if (ptr) chests[id] = ptr;
    else chests[id] = nullptr;
}

template<size_t map_size>
bool gltactics::map<map_size>::inSameRoom(Vector2 a, Vector2 b) {
    int x1 = int(a.x);
    int y1 = int(a.y);
    int x2 = int(b.x);
    int y2 = int(b.y);
    if (x1 > x2) {
        int tx = x2;
        int ty = y2;
        x2 = x1;
        y2 = y1;
        x1 = tx;
        y1 = ty;
    }
    int rh = y2 - y1;
    int rw = x2 - x1;
    for (int rx = 0; rx < rw; rx++) {
        int ry = int(std::round(float(rh) / float(rw) * float(rx)));
        size_t x = x1 + rx;
        size_t y = y1 + ry;
        if ((*this)[{y, x}].tileType == WALL) return false;
    }
    return true;
}


template<size_t map_size>
Vector2 gltactics::map<map_size>::doorInRoom(Vector2 point) {
    std::optional<std::pair<Rectangle, Vector2>> door;
    for (auto &k : doorsSections) {
        if (rectContains(k.first, point) && (!door || door->first.width > k.first.width || door->first.height > k.first.height))
            door = k;
    }
    assert(bool(door));
    return std::get<1>(*door);
}

size_t hash(Rectangle r) {
    return uint64_t(uint16_t(r.x)) << 48u | uint64_t(uint16_t(r.y)) << 32u | uint64_t(uint16_t(r.width)) << 16u |
           uint16_t(r.height);
}

bool operator<(const Rectangle a, const Rectangle b) {
    auto hashA = hash(a);
    auto hashB = hash(b);
    return hashA < hashB;
}


template<size_t map_size>
void gltactics::map<map_size>::addSection(Rectangle rect, Vector2 point) {
    doorsSections[rect] = point;
}

bool gltactics::rectContains(const Rectangle &a, const Rectangle &b) {
    bool hcontained = a.x >= b.x && (a.x + a.width) <= (b.x + b.width);
    bool vcontained = a.y >= b.y && (a.y + a.height) <= (b.y + b.height);
    return hcontained && vcontained;
}

template<size_t map_size>
void gltactics::overMapRange(gltactics::character<map_size> &playerCharacter, const rangeFunction &mapFunction,
                             bool &exitLoop) {
    size_t xMin = std::max((size_t) 0, (size_t) playerCharacter.position().x - 1);
    size_t xMax = std::min(map_size - 1, (size_t) playerCharacter.position().x + 1);
    size_t yMin = std::max((size_t) 0, (size_t) playerCharacter.position().y - 1);
    size_t yMax = std::min(map_size - 1, (size_t) playerCharacter.position().y + 1);
    for (size_t x = xMin; x <= xMax; x++) {
        for (size_t y = yMin; y <= yMax; y++) {
            mapFunction(x, y, playerCharacter.parent(), exitLoop);
            if (exitLoop) return;
        }
    }
}

template<size_t map_size>
bool gltactics::overMapRange(gltactics::character<map_size> &playerCharacter, const rangeFunction &mapFunction) {
    bool breakLoop = false;
    overMapRange(playerCharacter, mapFunction, breakLoop);
    return breakLoop;
}

bool gltactics::rectContains(const Rectangle &rect, const Vector2 &point) {
    bool hcontained = point.x >= rect.x && point.x <= (rect.x + rect.width);
    bool vcontained = point.y >= rect.y && point.y <= (rect.y + rect.height);
    return hcontained && vcontained;
}

template bool
gltactics::overMapRange<gltactics::DEFAULT_MAPSIZE>(gltactics::character<gltactics::DEFAULT_MAPSIZE> &playerCharacter,
                                                    const rangeFunction &mapFunction);

template
class gltactics::map<gltactics::DEFAULT_MAPSIZE>;
