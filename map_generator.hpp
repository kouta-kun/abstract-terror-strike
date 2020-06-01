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
      std::vector<gltactics::map<>> mapList {};
      bool hFlip = true; // flips on every map generation to build continuously playable maps
      std::array<int64_t, 2> lastExit = {-1, -1};
    public:
        size_t verticalSplit(size_t x, size_t y, size_t width, size_t height, size_t horDoor = -1);

        void horizontalSplit(size_t x, size_t y, size_t width, size_t height, size_t vertDoor = -1);

        std::vector<gltactics::attribute> placeChests(bool onLeftSide);

        void placeDoors(const std::vector<gltactics::attribute> &doorTypes, bool onRightSide, size_t vsplit);

        map_generator(int seed);

        gltactics::map<map_size> buildMap();

        void placeExit(bool onRightSide);

    };

};

#endif //ABSTRACT_TERROR_STRIKE_MAP_GENERATOR_HPP
