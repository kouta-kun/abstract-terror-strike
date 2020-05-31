//
// Created by Bedroom1 on 19/05/2020.
//

#ifndef ABSTRACT_TERROR_STRIKE_MAP_GENERATOR_HPP
#define ABSTRACT_TERROR_STRIKE_MAP_GENERATOR_HPP
#include <cstdint>
#include "map.hpp"
namespace gltactics {
    class map_generator {
      static const size_t map_size = gltactics::DEFAULT_MAPSIZE;
      gltactics::tile m[gltactics::DEFAULT_MAPSIZE][gltactics::DEFAULT_MAPSIZE] = {0};
      std::mt19937_64 generator;
      gltactics::chest *chests[8] = {nullptr};
    public:
        void vertical_split(ssize_t x, ssize_t y, ssize_t width, ssize_t height, ssize_t hor_door = -1);

        void horizontal_split(ssize_t x, ssize_t y, ssize_t width, ssize_t height, ssize_t vert_door = -1);

        std::vector<gltactics::attribute> place_chests();

        void placeDoors(const std::vector<gltactics::attribute> &doorTypes);

        map_generator(int seed);

        gltactics::map<map_size> build_map();
    };

};

#endif //ABSTRACT_TERROR_STRIKE_MAP_GENERATOR_HPP
