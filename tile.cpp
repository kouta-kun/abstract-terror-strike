#include "map.hpp"

gltactics::tile::operator uint16_t() const { return ((uint16_t) tileType << 8u) | ((uint8_t) attributeType); }

gltactics::tile::tile() {
    tileType = AIR;
    attributeType = (attribute) 0;
}

gltactics::tile::tile(uint16_t tile) {
    tileType = (type) (tile >> 8u);
    attributeType = (attribute) (tile & 255u);
}

bool gltactics::tile::isHorizontal() const {
    return attributeType & HORIZONTAL;
}

bool gltactics::tile::isOpen() const {
    return attributeType & OPEN;
}

bool gltactics::tile::isLocked() const {
    return attributeType & LOCKED;
}

bool gltactics::tile::isRed() const {
    return attributeType & _RED;
}

bool gltactics::tile::isBlue() const {
    return attributeType & _BLUE;
}

bool gltactics::tile::isGreen() const {
    return attributeType & _GREEN;
}