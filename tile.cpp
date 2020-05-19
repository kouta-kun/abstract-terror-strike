#include "map.hpp"

gltactics::tile::operator uint8_t() const { return ((uint8_t) tileType << 4u) | ((uint8_t) attributeType); }

gltactics::tile::tile() {
    tileType = AIR;
    attributeType = (attribute) 0;
}

gltactics::tile::tile(uint8_t tile) {
    tileType = (type) (tile >> 4u);
    attributeType = (attribute) (tile & 15u);
}

bool gltactics::tile::isHorizontal() const {
    return attributeType & HORIZONTAL;
}

bool gltactics::tile::isOpen() const {
    return attributeType & OPEN;
}