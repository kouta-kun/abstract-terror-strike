#ifndef ABSTRACT_TERROR_STRIKE_GHOST_H
#define ABSTRACT_TERROR_STRIKE_GHOST_H

#include <raylib.h>
#include "constants.hpp"
#include "map.hpp"


namespace gltactics {
    template<size_t map_size = DEFAULT_MAPSIZE>
    class ghost {
        std::reference_wrapper<map<map_size>> _map;
        Color _color;
        Vector2 _position;
        Vector2 target;
        std::reference_wrapper<std::mt19937_64> generator;
        int framesAlive = 0;

    public:
        ghost(Color color, Vector2 position, map<map_size> &map, std::mt19937_64 &generator);

        void stepAi();

        [[nodiscard]] std::vector<direction> directionsToward(const Vector2 &toward) const;

        void move(gltactics::direction direction);

        Vector3 position3D();

        Color color();
        ghost<map_size> &operator=(map<map_size> &newMap);
    };

};

#endif //ABSTRACT_TERROR_STRIKE_GHOST_H
