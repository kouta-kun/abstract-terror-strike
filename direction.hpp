//
// Created by kouta on 24/7/20.
//

#ifndef ABSTRACT_TERROR_STRIKE_DIRECTION_HPP
#define ABSTRACT_TERROR_STRIKE_DIRECTION_HPP

#include "constants.hpp"

namespace gltactics {
// layout[current_position + movementDirection] da referencia al bloque en esa direccion
    enum direction {
        up = -gltactics::DEFAULT_MAPSIZE,
        left = -1,
        right = 1,
        down = gltactics::DEFAULT_MAPSIZE
    };
};
#endif //ABSTRACT_TERROR_STRIKE_DIRECTION_HPP
