//
// Created by Bedroom1 on 30/05/2020.
//

#include "character.hpp"
#include "direction.hpp"

template<size_t map_size>
void gltactics::character<map_size>::openDoor(size_t x, size_t y, gltactics::map<map_size> &mapRef) {
    tile &tile = mapRef[{y, x}];
    type tileType = tile.tileType;
    if (tileType == DOOR) {
        attribute &attributeType = tile.attributeType;
        bool isLocked = attributeType & LOCKED;
        if (!isLocked) {
            attributeType = (attribute) (attributeType ^
                                         OPEN);
        }
    }
}

template<size_t map_size>
void gltactics::character<map_size>::useChest(size_t x, size_t y, gltactics::map<map_size> &mapRef) {
    tile &tile = mapRef[{y, x}];
    type &tileType = tile.tileType;
    if (tileType == CHEST) {
        uint8_t chest_id = tile.chest_id;
        gltactics::chest chest = *mapRef.getChest(chest_id);
        tileType = AIR;
        gltactics::item new_item = chest.pick_item();
        _inventory.insert(new_item);
    }
}

template<size_t map_size>
const std::array<size_t, 2> &gltactics::character<map_size>::position() const {
    return _position;
}

template<size_t map_size>
bool gltactics::character<map_size>::move(direction dir) {
    size_t destination = (size_t) (_position[0] * map_size + _position[1]) + dir;
    map<map_size> &wrapper = _map;
    type blockType = wrapper[destination].tileType;
    if (blockType == AIR || (blockType == DOOR && (wrapper[destination].attributeType & OPEN) > 0) ||
        blockType == EXIT) {
        _position[0] = int(destination / map_size);
        _position[1] = int(destination % map_size);
        return true;
    }
    return false;
}

template<size_t map_size>
void gltactics::character<map_size>::useEnvironment() {
    overMapRange(*this,
                 [&](size_t x, size_t y, gltactics::map<map_size> &mapRef, bool &_) {
                     useChest(x, y, mapRef);
                     openDoor(x, y, mapRef);
                 });
}

template<size_t map_size>
void gltactics::character<map_size>::useItems() {
    start:
    for (gltactics::item i : this->_inventory) {
        if (i.get_use_fn()(this)) {
            _inventory.extract(i);
            goto start;
        }
    }
}

template<size_t map_size>
std::set<int> gltactics::character<map_size>::getInventoryList() const {
    std::set<int> ids;
    for (auto &item : this->_inventory) {
        ids.insert(item.id);
    }
    return ids;
}

template<size_t map_size>
gltactics::map<map_size> &gltactics::character<map_size>::parent() {
    return _map;
}

template<size_t map_size>
gltactics::character<map_size>::character(std::array<size_t, 2> position, gltactics::map<map_size> &map)
        : _position{position}, _map{map} {}

template<size_t map_size>
gltactics::character<map_size> &gltactics::character<map_size>::operator=(gltactics::map<map_size> &newMap) {
    this->_map = newMap;
    return *this;
}

template<size_t map_size>
gltactics::character<map_size> &gltactics::character<map_size>::operator=(std::array<size_t, 2> newPosition) {
    _position = newPosition;
    return *this;
}

template
class gltactics::character<>;