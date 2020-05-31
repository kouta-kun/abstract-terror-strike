//
// Created by Bedroom1 on 30/05/2020.
//

#include "character.hpp"

template<ssize_t map_size>
void gltactics::character<map_size>::openDoor(ssize_t x, ssize_t y, gltactics::map<map_size> &mapRef) {
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

template<ssize_t map_size>
void gltactics::character<map_size>::useChest(ssize_t x, ssize_t y, gltactics::map<map_size> &mapRef) {
    tile &tile = mapRef[{y, x}];
    type &tileType = tile.tileType;
    if (tileType == CHEST) {
        uint8_t chest_id = tile.chest_id;
        gltactics::chest chest = *mapRef.getChest(chest_id);
        tileType = AIR;
        gltactics::item new_item = chest.pick_item();
        while(_inventory.find(new_item) != _inventory.end()) {
            new_item = chest.pick_item();
        }
        _inventory.insert(new_item);
    }
}

template<ssize_t map_size>
void gltactics::character<map_size>::setPosition(Vector2 position) {
    this->_position = position;
}

template<ssize_t map_size>
Vector3 gltactics::character<map_size>::position3D() const {
    return Vector3{_position.x, 0.0f, _position.y};
}

template<ssize_t map_size>
const Vector2 &gltactics::character<map_size>::position() const {
    return _position;
}

template<ssize_t map_size>
bool gltactics::character<map_size>::move(direction dir) {
    size_t destination = (size_t) (_position.y * map_size + _position.x) + dir;
    type blockType = _map[destination].tileType;
    if (blockType == AIR || (blockType == DOOR && (_map[destination].attributeType & OPEN) > 0)) {
        _position.y = float(destination / map_size);
        _position.x = float(destination % map_size);
        return true;
    }
    return false;
}

template<ssize_t map_size>
void gltactics::character<map_size>::useEnvironment() {
    overMapRange(*this,
                 [&](ssize_t x, ssize_t y, gltactics::map<map_size> &mapRef, bool &_) {
                     useChest(x, y, mapRef);
                     openDoor(x, y, mapRef);
                 });
}

template<ssize_t map_size>
void gltactics::character<map_size>::useItems() {
    start:
    for(gltactics::item i : this->_inventory) {
        if(i.get_use_fn()(this)) {
            _inventory.extract(i);
            goto start;
        }
    }
}

template<ssize_t map_size>
std::set<int> gltactics::character<map_size>::getInventoryList() const {
    std::set<int> ids;
    for (auto &item : this->_inventory) {
        ids.insert(item.id);
    }
    return ids;
}

template<ssize_t map_size>
void gltactics::character<map_size>::setColor(Color color) {
    this->_color = color;
}

template<ssize_t map_size>
const Color &gltactics::character<map_size>::color() const {
    return _color;
}

template<ssize_t map_size>
gltactics::map<map_size> &gltactics::character<map_size>::map() {
    return _map;
}

template<ssize_t map_size>
gltactics::character<map_size>::character(Color color, Vector2 position, gltactics::map<map_size> &map) : _color{color},
                                                                                                          _position{
                                                                                                                  position},
                                                                                                          _map{map} {}

template
class gltactics::character<>;