#ifndef ABSTRACT_TERROR_STRIKE_GHOST_H
#define ABSTRACT_TERROR_STRIKE_GHOST_H

#include "constants.hpp"
#include "map.hpp"
#include "direction.hpp"


namespace gltactics {
    template<size_t map_size = DEFAULT_MAPSIZE>
    class ghost {
        std::reference_wrapper<map<map_size>> _map;
        Color _color;
        std::array<size_t, 2> _position;
        std::array<size_t, 2> target;
        std::reference_wrapper<std::mt19937_64> generator;
        int framesAlive = 0;
        int framesWithTarget = 0;

    public:
        ghost(Color color, std::array<size_t, 2> position, map<map_size> &map, std::mt19937_64 &generator);

        void stepAi();

        [[nodiscard]] std::vector<direction> directionsToward(const std::array<size_t, 2> &toward) const;

        void move(direction direction);

        std::array<size_t, 2> position() const;

        ghost<map_size> &operator=(map<map_size> &newMap);
    };

};

#endif //ABSTRACT_TERROR_STRIKE_GHOST_H
