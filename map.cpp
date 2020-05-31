//
// Created by Bedroom1 on 19/05/2020.
//

#include "map.hpp"
#include "character.hpp"

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

template bool
gltactics::overMapRange<gltactics::DEFAULT_MAPSIZE>(gltactics::character<gltactics::DEFAULT_MAPSIZE> &playerCharacter,
                                                    const rangeFunction &mapFunction);

template
class gltactics::map<gltactics::DEFAULT_MAPSIZE>;
