#ifndef ABSTRACT_TERROR_STRIKE_CHARACTER_HPP
#define ABSTRACT_TERROR_STRIKE_CHARACTER_HPP

#include <set>
#include "map.hpp"
#include "chest.hpp"
#include "direction.hpp"

namespace gltactics {
    template<size_t map_size = DEFAULT_MAPSIZE>
    class character {
        std::reference_wrapper<map<map_size>> _map;
        std::array<size_t, 2> _position;
        std::set<gltactics::item> _inventory;

        void useChest(size_t x, size_t y, gltactics::map<map_size> &mapRef);

        void openDoor(size_t x, size_t y, gltactics::map<map_size> &mapRef);

    public:
        character(std::array<size_t, 2> position, map<map_size> &map);

        map<map_size> &parent();

        [[nodiscard]] std::set<int> getInventoryList() const;

        void useEnvironment();

        bool move(direction dir);

        [[nodiscard]] const std::array<size_t, 2> &position() const;

        void useItems();

        template<gltactics::attribute onAttribute>
        static bool openDoors(gltactics::character<map_size> *character);

        character<map_size> &operator=(map<map_size> &newMap);

        character<map_size> &operator=(std::array<size_t, 2> newPosition);
    };

    template<size_t map_size>
    template<attribute onAttribute>
    bool gltactics::character<map_size>::openDoors(character<map_size> *character) {
        bool consumed = false;
        overMapRange(*character,
                     [&](size_t x, size_t y, gltactics::map<map_size> &mapRef, bool &_) {
                         tile &tile = mapRef[{y, x}];
                         if (tile.tileType == DOOR && tile.attributeType & onAttribute && tile.attributeType & LOCKED) {
                             consumed = true;
                             tile.attributeType =
                                     (attribute) (tile.attributeType ^ (LOCKED));
                         }
                     });
        return consumed;
    }
};
#endif
