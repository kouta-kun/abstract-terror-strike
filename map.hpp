#ifndef ABSTRACT_TERROR_STRIKE_MAP_HPP
#define ABSTRACT_TERROR_STRIKE_MAP_HPP

#include <cstdint>
#include <fstream>
#include <array>
#include <random>
#include <cassert>
#include <optional>
#include <functional>
#include "constants.hpp"
#include "chest.hpp"

namespace gltactics {

  enum type : uint8_t {
		       AIR = 0,
		       WALL = 1,
		       DOOR = 2,
		       CHEST = 3
  };

  enum attribute : uint8_t {
			    HORIZONTAL = 1,
			    OPEN = 2,
			    LOCKED = 4,
			    _RED = 8,
			    _BLUE = 16,
			    _GREEN = 32
  };

  struct tile {
    type tileType;
    union {
      attribute attributeType;
      uint8_t chest_id;
    };

    operator uint16_t() const;

    tile();

    tile(uint16_t tile);

    [[nodiscard]] bool isHorizontal() const;

    [[nodiscard]] bool isOpen() const;
  };


  template<ssize_t map_size = DEFAULT_MAPSIZE>
  class map {
    tile *layout = new tile[map_size * map_size]; // mapa cuadrado de NxN

    chest **chests = new chest*[8]{nullptr};

  public:
    map(uint8_t fillMap);

    map(std::ifstream inputFile);

    // map[{y, x}]
    tile &operator[](std::array<ssize_t, 2> index);

    tile &operator[](ssize_t index);

    std::optional<chest> getChest(ssize_t id);
    void setChest(ssize_t id, chest *ptr);
  };

  // layout[current_position + direction] da referencia al bloque en esa direccion
  enum direction : ssize_t {
			    up = -DEFAULT_MAPSIZE,
			    left = -1,
			    right = 1,
			    down = DEFAULT_MAPSIZE
  };

  typedef std::function<void(ssize_t,ssize_t,gltactics::map<>&,bool&)> rangeFunction;

  template<ssize_t map_size>
  void overMapRange(gltactics::character<map_size> &playerCharacter, rangeFunction mapFunction);
}
#endif
