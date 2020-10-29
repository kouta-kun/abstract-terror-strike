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
#include <set>
#include "constants.hpp"
#include "chest.hpp"

namespace gltactics {

    struct room_def {
        std::optional<std::reference_wrapper<room_def>> parent;
        size_t x, y;
        size_t w, h;

        room_def(size_t x, size_t y, size_t w, size_t h);

        room_def(room_def &parent, size_t x, size_t y, size_t w, size_t h);

        bool contains(const room_def &b);
    };

    std::set<room_def> getParentSet(const room_def &);

    std::partial_ordering operator<=>(const room_def &a, const room_def &b);

    bool operator==(const room_def &a, const room_def &b);;

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

        std::array<std::optional<gltactics::chest>, 8> chests = {std::optional<gltactics::chest>()};

        std::map<gltactics::room_def, std::array<size_t, 2>> doorsSections;

        gltactics::room_def parentRoom{0, 0, map_size, map_size};


    public:
        map(uint8_t fillMap);

        map(std::ifstream inputFile);

        // map[{y, x}]
        tile &operator[](std::array<size_t, 2> index);

        tile &operator[](size_t index);

        std::optional<chest> getChest(size_t id);

        void setChest(size_t id, std::optional<chest> ptr);

        bool inSameRoom(std::array<size_t, 2> a, std::array<size_t, 2> b);

        std::array<size_t, 2> doorInRoom(std::array<size_t, 2> point) const;

        room_def deepestRoomAtPoint(std::array<size_t, 2> point) const;


        void addSection(room_def rect, std::array<size_t, 2> point);
    };

    bool rectContains(const room_def &rect, const std::array<size_t, 2> &point);

    typedef std::function<void(size_t, size_t, gltactics::map<> &, bool &)> rangeFunction;

    constexpr std::array<attribute, 3> lockDoorTags = {_RED, _BLUE, _GREEN};

    template<size_t map_size>
    void
    overMapRange(gltactics::character<map_size> &playerCharacter, const rangeFunction &mapFunction, bool &exitedEarly);

    template<size_t map_size>
    bool overMapRange(gltactics::character<map_size> &playerCharacter, const rangeFunction &mapFunction);
}
#endif
