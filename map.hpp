#ifndef ABSTRACT_TERROR_STRIKE_MAP_HPP
#define ABSTRACT_TERROR_STRIKE_MAP_HPP

#include <cstdint>
#include <fstream>
#include <array>
#include <random>
#include <cassert>
#include <optional>
#include <functional>
#include <map>
#include "constants.hpp"
#include "chest.hpp"
#include "raylib.h"

namespace gltactics {

    enum type : uint8_t {
        AIR = 0,
        WALL = 1,
        DOOR = 2,
        CHEST = 3,
        EXIT = 4
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

        [[nodiscard]] bool isLocked() const;

        [[nodiscard]] bool isRed() const;

        [[nodiscard]] bool isBlue() const;

        [[nodiscard]] bool isGreen() const;

    };


    template<size_t map_size = DEFAULT_MAPSIZE>
    class map {
        tile *layout = new tile[map_size * map_size]; // mapa cuadrado de NxN

        chest **chests = new chest *[8]{nullptr};

        std::map<Rectangle, Vector2> doorsSections;


    public:
        map(uint8_t fillMap);

        map(std::ifstream inputFile);

        // map[{y, x}]
        tile &operator[](std::array<size_t, 2> index);

        tile &operator[](size_t index);

        std::optional<chest> getChest(size_t id);

        void setChest(size_t id, chest *ptr);

        bool inSameRoom(Vector2 a, Vector2 b);

        Vector2 doorInRoom(Vector2 point);

        void addSection(Rectangle rect, Vector2 point);
    };

    // layout[current_position + direction] da referencia al bloque en esa direccion
    enum direction {
        up = -DEFAULT_MAPSIZE,
        left = -1,
        right = 1,
        down = DEFAULT_MAPSIZE
    };

    bool rectContains(const Rectangle &rect, const Vector2 &point);

    bool rectContains(const Rectangle &a, const Rectangle &b);

    typedef std::function<void(size_t, size_t, gltactics::map<> &, bool &)> rangeFunction;

    constexpr std::array<attribute, 3> lockDoorTags = {_RED, _BLUE, _GREEN};

    template<size_t map_size>
    void
    overMapRange(gltactics::character<map_size> &playerCharacter, const rangeFunction &mapFunction, bool &exitedEarly);

    template<size_t map_size>
    bool overMapRange(gltactics::character<map_size> &playerCharacter, const rangeFunction &mapFunction);
}
#endif
