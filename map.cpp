//
// Created by Bedroom1 on 19/05/2020.
//

#include "map.hpp"

template<ssize_t map_size>
gltactics::map<map_size>::map(uint8_t fillMap) {
    for (int y = 0; y < map_size; y++) {
        for (int x = 0; x < map_size; x++) {
            layout[y * map_size + x] = fillMap; // llenar el mapa con este valor uint8_t
        }
    }
}

template<ssize_t map_size>
gltactics::map<map_size>::map(std::ifstream inputFile) {
    char line[map_size + 3];
    for (int y = 0; y < map_size; y++) {
        inputFile.getline(line, map_size + 2); // leemos una linea de hasta 18 caracteres
        for (int x = 0; x < map_size; x++) {
            layout[y * map_size + x] = line[x] - '0'; // almacenamos el valor como esta en el txt
        }
    }
}

template<ssize_t map_size>
gltactics::tile &gltactics::map<map_size>::operator[](std::array<ssize_t, 2> index) {
    return layout[index[0] * map_size + index[1]];
}

template<ssize_t map_size>
gltactics::tile &gltactics::map<map_size>::operator[](ssize_t index) {
    return layout[index];
}

template class gltactics::map<gltactics::DEFAULT_MAPSIZE>;