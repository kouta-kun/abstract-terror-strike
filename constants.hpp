#ifndef _CONSTANTS_HPP
#define _CONSTANTS_HPP

#include <cstdint>
#include <fstream>
#include <array>
#include <random>
#include <cassert>
#include <optional>
#include <functional>


namespace gltactics {
    enum Color {
        RED,
        BLUE,
        GREEN
    };
    constexpr uint8_t DEFAULT_MAPSIZE = 24;
    constexpr std::array<Color, 3> keyColors = {RED, BLUE, GREEN};
};
#endif
