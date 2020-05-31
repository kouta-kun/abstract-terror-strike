#ifndef ABSTRACT_TERROR_STRIKE_CHARACTER_HPP
#define ABSTRACT_TERROR_STRIKE_CHARACTER_HPP

#include <set>
#include "map.hpp"
#include "chest.hpp"
#include "raylib.h"

namespace gltactics {
    template<size_t map_size = DEFAULT_MAPSIZE>
    class character {
        map<map_size> &_map;
        Color _color;
        Vector2 _position;
        std::set<gltactics::item> _inventory;

        void useChest(size_t x, size_t y, gltactics::map<map_size> &mapRef);

        void openDoor(size_t x, size_t y, gltactics::map<map_size> &mapRef);

    public:
        character(Color color, Vector2 position, map<map_size> &map);

        map<map_size> &parent();

        [[nodiscard]] const Color &color() const;

        void setColor(Color color);

        [[nodiscard]] std::set<int> getInventoryList() const;

        void useEnvironment();

        bool move(direction dir);

        [[nodiscard]] const Vector2 &position() const;

        [[nodiscard]] Vector3 position3D() const;

        void setPosition(Vector2 position);

        void useItems();

        template<gltactics::attribute onAttribute>
        static bool openDoors(gltactics::character<> *character);
    };

    template<size_t map_size>
    template<attribute onAttribute>
    bool gltactics::character<map_size>::openDoors(character<> *character) {
        bool consumed = false;
        overMapRange(*character,
                     [&](size_t x, size_t y, gltactics::map<map_size> &mapRef, bool &_) {
                         tile &tile = mapRef[{y, x}];
                         if (tile.tileType == DOOR) {
                             if (tile.attributeType & onAttribute) {
                                 consumed = true;
                                 tile.attributeType =
                                         (attribute) (tile.attributeType ^ (LOCKED));
                             }
                         }
                     });
        return consumed;
    }
};
#endif
