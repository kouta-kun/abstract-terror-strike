//
// Created by Bedroom1 on 19/05/2020.
//

#ifndef ABSTRACT_TERROR_STRIKE_MAP_GENERATOR_HPP
#define ABSTRACT_TERROR_STRIKE_MAP_GENERATOR_HPP
#include <cstdint>
#include <list>
#include <set>
#include "map.hpp"
namespace gltactics {
    class map_generator {
      static const size_t map_size = gltactics::DEFAULT_MAPSIZE;
      gltactics::tile m[gltactics::DEFAULT_MAPSIZE][gltactics::DEFAULT_MAPSIZE] = {0};
      std::optional<std::reference_wrapper<std::mt19937_64>> generator;
      std::array<std::optional<gltactics::chest>,8> chests = {std::optional<gltactics::chest>()};
      std::vector<gltactics::map<>> mapList {};
      bool hFlip = true; // flips on every map generation to build continuously playable maps
      std::array<int64_t, 2> lastExit = {-1, -1};
      std::list<std::array<size_t, 2>> doors;
      std::list<room_def> sectors;
      void initialize();
      gltactics::map<map_size> copyMap() const;
    public:
        size_t verticalSplit(size_t x, size_t y, size_t width, size_t height, size_t horDoor = -1);

        void horizontalSplit(size_t x, size_t y, size_t width, size_t height, size_t vertDoor = -1);

        std::vector<gltactics::attribute> placeChests(bool onLeftSide);

        void placeDoors(const std::vector<gltactics::attribute> &doorTypes, bool onRightSide, size_t vsplit);

        map_generator(std::mt19937_64 &generator);
        map_generator(std::nullptr_t);
        map_generator(const map_generator &mapGenerator);

        gltactics::map<map_size> buildMap();

        void placeExit(bool onRightSide);

        int currentFloor();
    };

};

#endif //ABSTRACT_TERROR_STRIKE_MAP_GENERATOR_HPP
