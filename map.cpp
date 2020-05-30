//
// Created by Bedroom1 on 19/05/2020.
//

#include "map.hpp"
#include "character.hpp"

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

template<ssize_t map_size>
std::optional<gltactics::chest> gltactics::map<map_size>::getChest(ssize_t id) {
  if(this->chests[id] != nullptr) {
    return *(this->chests[id]);
  } else return std::optional<gltactics::chest>();
}

template<ssize_t map_size>
void gltactics::map<map_size>::setChest(ssize_t id, gltactics::chest *ptr) {
  if(ptr) chests[id] = ptr;
  else chests[id] = nullptr;
}

template<ssize_t map_size>
void gltactics::overMapRange(gltactics::character<map_size> &playerCharacter, rangeFunction mapFunction) {
  bool exitLoop = false;
  for (ssize_t x = std::max((ssize_t) 0, (ssize_t) playerCharacter.position().x - 1);
       x < std::min(map_size, (ssize_t) playerCharacter.position().x + 2); x++) {
    for (ssize_t y = std::max((ssize_t) 0, (ssize_t) playerCharacter.position().y - 1);
	 y < std::min(map_size, (ssize_t) playerCharacter.position().y + 2); y++) {
      mapFunction(x, y, playerCharacter.map(), exitLoop);
      if (exitLoop) return;
    }
  }
}

template void gltactics::overMapRange<gltactics::DEFAULT_MAPSIZE>(gltactics::character<gltactics::DEFAULT_MAPSIZE> &playerCharacter, rangeFunction mapFunction);

template class gltactics::map<gltactics::DEFAULT_MAPSIZE>;
