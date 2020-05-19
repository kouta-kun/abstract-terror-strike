#ifndef ABSTRACT_TERROR_STRIKE_MAP_HPP
#define ABSTRACT_TERROR_STRIKE_MAP_HPP

#include <cstdint>
#include <fstream>
#include <array>
#include <random>
#include <cassert>

namespace gltactics {

    constexpr ssize_t DEFAULT_MAPSIZE = 24;

    enum type : uint8_t {
        AIR = 0,
        WALL = 1,
        DOOR = 2
    };

    enum attribute : uint8_t {
        HORIZONTAL = 1,
        OPEN = 2
    };

    struct tile {
        type tileType: 4;
        attribute attributeType: 4;

        operator uint8_t() const;

        tile();

        tile(uint8_t tile);

        [[nodiscard]] bool isHorizontal() const;

        [[nodiscard]] bool isOpen() const;
    };


    template<ssize_t map_size = DEFAULT_MAPSIZE>
    class map {
        tile *layout = new tile[map_size * map_size]; // mapa cuadrado de NxN

    public:
        map(uint8_t fillMap);

        map(std::ifstream inputFile);

        // map[{y, x}]
        tile &operator[](std::array<ssize_t, 2> index);

        tile &operator[](ssize_t index);
    };

    // layout[current_position + direction] da referencia al bloque en esa direccion
    enum direction : ssize_t {
        up = -DEFAULT_MAPSIZE,
        left = -1,
        right = 1,
        down = DEFAULT_MAPSIZE
    };

}
#endif
