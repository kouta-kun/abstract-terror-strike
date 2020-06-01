//
// Created by Bedroom1 on 19/05/2020.
//

#include "map_generator.hpp"
#include "character.hpp"

size_t
gltactics::map_generator::verticalSplit(size_t x, size_t y, size_t width, size_t height, size_t horDoor) {
    if (height <= 6) return -1;
    size_t leftRoomWidth; // separes room into left and right rooms
    size_t wRandMod = 2;
    std::uniform_int_distribution<> wDis(-wRandMod, wRandMod);
    size_t hRandMod = (height - 1);
    std::uniform_int_distribution<> hDis(0, hRandMod);
    bool vsplitDoorColission;
    bool lastStairColission;
    size_t iterCount = 0;
    do {
        leftRoomWidth = (width / 2) + (wDis(generator));
        vsplitDoorColission = horDoor >= 0 && leftRoomWidth == horDoor;
        lastStairColission =
                lastExit[0] >= 0 && lastExit[0] >= x && lastExit[0] <= x + width && lastExit[1] == leftRoomWidth + x;
        if(iterCount > 20) return -1;
    } while (vsplitDoorColission || lastStairColission);
    size_t vertDoor;
    iterCount = 0;
    do {
        vertDoor = hDis(generator); // door on the line between both rooms
        if (++iterCount > 15) {
            return -1;
        }
    } while (vertDoor <= 2 || vertDoor >= (map_size - 2) ||
             m[y + vertDoor][x + leftRoomWidth - 1].tileType != gltactics::AIR ||
             m[y + vertDoor][x + leftRoomWidth + 1].tileType != gltactics::AIR);
    for (size_t y2 = 0; y2 < height; y2++) {
        m[y + y2][x + leftRoomWidth].tileType = gltactics::WALL;
    }
    m[y + vertDoor][x + leftRoomWidth].tileType = gltactics::DOOR;
    size_t rightRoomWidth = width - leftRoomWidth;
    if (!(leftRoomWidth * height <= 16 || rightRoomWidth * height <= 16)) {
      horizontalSplit(x, y, leftRoomWidth, height, vertDoor);
      horizontalSplit(leftRoomWidth + x, y, rightRoomWidth, height, vertDoor);
    }
    return x + leftRoomWidth;
}

void
gltactics::map_generator::horizontalSplit(size_t x, size_t y, size_t width, size_t height, size_t vertDoor) {
    if (width <= 6) return;
    size_t topRoomHeight; // separates room into top and bottom rooms
    size_t wRandMod = width - 1;
    std::uniform_int_distribution<> wDis(1, wRandMod);
    size_t hRandMod = 2;
    std::uniform_int_distribution<> hDis(-hRandMod, hRandMod);
    bool hsplitDoorColission;
    bool lastStairColission;
    do {
        topRoomHeight = (height / 2) + (hDis(generator));
        hsplitDoorColission = vertDoor >= 0 && topRoomHeight == vertDoor;
        lastStairColission = lastExit[1] >= y && lastExit[1] <= y + height && lastExit[1] != topRoomHeight + x;
    } while (hsplitDoorColission || lastStairColission);
    for (size_t x2 = 0; x2 < width; x2++) {
        m[y + topRoomHeight][x + x2].tileType = gltactics::WALL;
    }
    size_t horDoor;
    do {
        horDoor = wDis(generator);
    } while (horDoor <= 2 || horDoor >= (map_size - 2) ||
             m[y + topRoomHeight - 1][x + horDoor].tileType != gltactics::AIR ||
             m[y + topRoomHeight + 1][x + horDoor].tileType != gltactics::AIR);
    m[y + topRoomHeight][x + horDoor].tileType = gltactics::DOOR;
    m[y + topRoomHeight][x + horDoor].attributeType = gltactics::HORIZONTAL;
    auto bottomRoomHeight = height - topRoomHeight;
    if (bottomRoomHeight * width <= 16 || topRoomHeight * width <= 16) return;
    verticalSplit(x, y, width, topRoomHeight, horDoor);
    verticalSplit(x, y + topRoomHeight, width, bottomRoomHeight, horDoor);
}

gltactics::map_generator::map_generator(int seed) : generator(seed) {

}

std::vector<gltactics::attribute> gltactics::map_generator::placeChests(bool onLeftSide) {
    std::uniform_int_distribution<size_t> dis(1, DEFAULT_MAPSIZE - 1);
    std::uniform_int_distribution<> chest_count_dis(1, 3);
    int chest_count = chest_count_dis(generator);
    chest_count_dis = std::uniform_int_distribution<>(0, 2);

    gltactics::item red_key(0x10,
                            character<>::
                            openDoors<gltactics::attribute::_RED>);

    gltactics::item blue_key(0x11,
                             character<>::
                             openDoors<gltactics::attribute::_BLUE>);

    gltactics::item green_key(0x12,
                              character<>::
                              openDoors<gltactics::attribute::_GREEN>);

    std::vector<gltactics::item> keys{red_key, blue_key, green_key};
    std::vector<gltactics::attribute> keysAttribute{_RED, _BLUE, _GREEN};

    std::vector<gltactics::attribute> usedKeys{};
    int xOffset = onLeftSide ? 0 : map_size / 2;
    for (int i = 0; i < chest_count; i++) {
        int keyIndex = chest_count_dis(generator);
        this->chests[i] = new chest(keys[keyIndex]);
        std::array<size_t, 2> chestTile{};
        do {
            chestTile = {dis(generator), xOffset + dis(generator) / 2};
        } while (m[chestTile[0]][chestTile[1]].tileType != gltactics::type::AIR);
        keys.erase(keys.begin() + keyIndex);
        usedKeys.push_back(keysAttribute[keyIndex]);
        keysAttribute.erase(keysAttribute.begin() + keyIndex);
        m[chestTile[0]][chestTile[1]].tileType = gltactics::type::CHEST;
        m[chestTile[0]][chestTile[1]].chest_id = i;
        chest_count_dis = std::uniform_int_distribution<>(0, 2 - i - 1);
    }

    return usedKeys;
}

void gltactics::map_generator::placeDoors(const std::vector<gltactics::attribute> &doorTypes, bool onRightSide, size_t vSplit) {
    std::vector<std::array<size_t, 2>> doorsToLock{};
    std::uniform_int_distribution<size_t> dis(50);
    int idx = 0;
    int xOffset = onRightSide ? vSplit + 1 : 1;
    while (idx != doorTypes.size()) {
        for (size_t x = xOffset; x < xOffset + map_size - vSplit - 1; x++) {
            for (size_t y = 0; y < gltactics::map_generator::map_size; y++) {
                if (this->m[y][x].tileType == gltactics::DOOR && idx < doorTypes.size() && dis(this->generator) > 40) {
                    doorsToLock.push_back({y, x});
                    idx++;
                }
            }
        }
    }
    for (int i = 0; i < doorTypes.size(); i++) {
        auto[y, x] = doorsToLock[i];
        auto &door = this->m[y][x];
        door.attributeType = static_cast<gltactics::attribute>(door.attributeType | (gltactics::attribute) (
                doorTypes[i] | gltactics::LOCKED));
    }
}


gltactics::map<gltactics::DEFAULT_MAPSIZE> gltactics::map_generator::buildMap() {
    std::fill(&m[0][0], &m[map_size - 1][map_size - 1] + 1, 0);
    for (int i = 0; i < map_size; i++) {
        m[0][i].tileType = gltactics::WALL;
        m[i][0].tileType = gltactics::WALL;
        m[map_size - 1][i].tileType = gltactics::WALL;
        m[i][map_size - 1].tileType = gltactics::WALL;
    }
    auto vsplit = verticalSplit(0, 0, map_size, map_size);
    auto doorTypes = placeChests(hFlip);
    placeDoors(doorTypes, hFlip, vsplit);
    placeExit(hFlip);
    gltactics::map<map_size> generatedMap{0};
    for (size_t y = 0; y < map_size; y++) {
        for (size_t x = 0; x < map_size; x++) {
            generatedMap[{y, x}] = m[y][x];
        }
    }
    for (int i = 0; i < 8; i++) generatedMap.setChest(i, chests[i]);
    hFlip = !hFlip;
    return generatedMap;
}

void gltactics::map_generator::placeExit(bool onRightSide) {
    std::uniform_int_distribution<size_t> dis(0, map_size / 2);
    size_t xOffset = onRightSide ? map_size / 2 + 1 : 1;
    size_t x, y;
    do {
        x = dis(generator) + xOffset;
        y = dis(generator) * 2;
    } while (m[y][x].tileType != AIR);
    this->lastExit = {y, x};
    m[y][x].tileType = gltactics::type::EXIT;
}
