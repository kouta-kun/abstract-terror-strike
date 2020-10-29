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
    return this->chests[id];
}

template<size_t map_size>
void gltactics::map<map_size>::setChest(size_t id, std::optional<gltactics::chest> ptr) {
    if (ptr) chests[id] = ptr;
    else chests[id] = std::optional<gltactics::chest>();
}

template<size_t map_size>
bool gltactics::map<map_size>::inSameRoom(std::array<size_t, 2> a, std::array<size_t, 2> b) {
//    int x1 = int(a.x);
//    int y1 = int(a.y);
//    int x2 = int(b.x);
//    int y2 = int(b.y);
//    if (x1 > x2) {
//        int tx = x2;
//        int ty = y2;
//        x2 = x1;
//        y2 = y1;
//        x1 = tx;
//        y1 = ty;
//    }
//    int rh = y2 - y1;
//    int rw = x2 - x1;
//    for (int rx = 0; rx < rw; rx++) {
//        int ry = int(std::round(float(rh) / float(rw) * float(rx)));
//        size_t x = x1 + rx;
//        size_t y = y1 + ry;
//        if ((*this)[{y, x}].tileType == WALL) return false;
//    }
//    return true;
// the previous implementation should have been faster but i'm not sure it works.
    return deepestRoomAtPoint(a) == deepestRoomAtPoint(b);
}


template<size_t map_size>
std::array<size_t, 2> gltactics::map<map_size>::doorInRoom(std::array<size_t, 2> point) const {
    std::optional<room_def> room = deepestRoomAtPoint(point);
    assert(bool(room));
    return doorsSections.at(*room);
}

template
        <size_t map_size>
gltactics::room_def gltactics::map<map_size>::deepestRoomAtPoint(std::array<size_t, 2> point) const {
    std::optional<gltactics::room_def> room;
    for (auto &k : this->doorsSections) {
        if (gltactics::rectContains(k.first, point) &&
            (!room || room->w > k.first.w || room->h > k.first.h))
            room = k.first;
    }
    return *room;
}

template<size_t map_size>
void gltactics::map<map_size>::addSection(room_def rect, std::array<size_t, 2> point) {
    doorsSections[rect] = point;
}

template<size_t map_size>
void gltactics::overMapRange(gltactics::character<map_size> &playerCharacter, const rangeFunction &mapFunction,
                             bool &exitLoop) {
    size_t xMin = std::max((size_t) 0, (size_t) playerCharacter.position()[1] - 1);
    size_t xMax = std::min(map_size - 1, (size_t) playerCharacter.position()[1] + 1);
    size_t yMin = std::max((size_t) 0, (size_t) playerCharacter.position()[0] - 1);
    size_t yMax = std::min(map_size - 1, (size_t) playerCharacter.position()[0] + 1);
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

bool gltactics::rectContains(const room_def &rect, const std::array<size_t, 2> &point) {
    bool hcontained = point[1] >= rect.x && point[1] <= (rect.x + rect.w);
    bool vcontained = point[0] >= rect.y && point[0] <= (rect.y + rect.h);
    return hcontained && vcontained;
}

template bool
gltactics::overMapRange<gltactics::DEFAULT_MAPSIZE>(gltactics::character<gltactics::DEFAULT_MAPSIZE> &playerCharacter,
                                                    const rangeFunction &mapFunction);

template
class gltactics::map<gltactics::DEFAULT_MAPSIZE>;

gltactics::room_def::room_def(size_t x, size_t y, size_t w, size_t h) : x(x), y(y), w(w), h(h) {}

gltactics::room_def::room_def(room_def &parent, size_t x, size_t y, size_t w, size_t h)
        : parent(parent), x(x), y(y), w(w), h(h) {}

std::set<gltactics::room_def>
gltactics::getParentSet(const gltactics::room_def &room) {
    std::set<gltactics::room_def> parentSet;
    std::reference_wrapper<const gltactics::room_def> iter = room;
    while ((bool) iter.get().parent) {
        const std::reference_wrapper<gltactics::room_def> &pointer = *(iter.get().parent);
        parentSet.insert(pointer.get());
        iter = pointer;
    }
    return parentSet;
}

bool gltactics::room_def::contains(const gltactics::room_def &b) {
    return gltactics::getParentSet(*this).contains(b);
}

std::partial_ordering gltactics::operator<=>(const room_def &a, const room_def &b) {
    if (a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h) return std::partial_ordering::equivalent;
    if (getParentSet(a).contains(b)) return std::partial_ordering::less;
    if (getParentSet(b).contains(a)) return std::partial_ordering::greater;
    return std::partial_ordering::unordered;
}

bool gltactics::operator==(const gltactics::room_def &a, const gltactics::room_def &b) {
    return (a <=> b) == std::partial_ordering::equivalent;
}
