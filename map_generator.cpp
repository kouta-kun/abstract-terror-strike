//
// Created by Bedroom1 on 19/05/2020.
//

#include <iostream>
#include "map_generator.hpp"
#include "character.hpp"
#include <cmath>

size_t
gltactics::map_generator::verticalSplit(size_t x, size_t y, size_t width, size_t height, size_t horDoor) {
    if (height <= 6) return -1;
    size_t leftRoomWidth = width / 2; // separes room into left and right rooms
    size_t wRandMod = 2;
    std::uniform_int_distribution<> wDis(-wRandMod, wRandMod);
    size_t hRandMod = (height - 1);
    std::uniform_int_distribution<> hDis(0, hRandMod);
    bool vsplitDoorColission;
    bool lastStairColission;
    size_t iterCount = 0;
    do {
        leftRoomWidth = (width / 2) + (wDis(generator->get()));
        vsplitDoorColission = false;
        for (int yOff = 0; yOff < height + 1; yOff++) {
            if (std::find(std::begin(doors), std::end(doors),
                          std::array<size_t, 2>({y + yOff, x + leftRoomWidth}))
                != std::end(doors)) {
                vsplitDoorColission = true;
                break;
            }
        }
        lastStairColission =
                lastExit[0] >= 0 && lastExit[1] == x + leftRoomWidth && lastExit[0] >= y && lastExit[0] <= y + height;
        if (++iterCount > 20) return -1;
    } while (vsplitDoorColission || lastStairColission);
    size_t vertDoor;
    iterCount = 0;
    do {
        vertDoor = hDis(generator->get()); // door on the line between both rooms
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
    doors.push_back({y + vertDoor, x + leftRoomWidth});
    sectors.push_back(gltactics::room_def({.x=x, .y=y, .w=width, .h=height}));
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
    size_t topRoomHeight = height / 2; // separates room into top and bottom rooms
    size_t wRandMod = width - 1;
    std::uniform_int_distribution<> wDis(1, wRandMod);
    size_t hRandMod = 2;
    std::uniform_int_distribution<> hDis(-hRandMod, hRandMod);
    bool hsplitDoorColission;
    bool lastStairColission;
    size_t iterCount = 0;
    do {
        topRoomHeight = (height / 2) + (hDis(generator->get()));
        hsplitDoorColission = false;
        for (int xOff = 0; xOff < width + 1; xOff++) {
            if (std::find(std::begin(doors), std::end(doors),
                          std::array<size_t, 2>({(y + topRoomHeight), (x + xOff)}))
                != std::end(doors)) {
                hsplitDoorColission = true;
                break;
            }
        }
        lastStairColission =
                lastExit[0] >= 0 && lastExit[0] == y + topRoomHeight && lastExit[1] >= x && lastExit[1] <= x + width;
        if (++iterCount > 15) return;
    } while (hsplitDoorColission || lastStairColission);
    for (size_t x2 = 0; x2 < width; x2++) {
        m[y + topRoomHeight][x + x2].tileType = gltactics::WALL;
    }
    size_t horDoor;
    iterCount = 0;
    do {
        horDoor = wDis(generator->get());
        if (++iterCount > 15) return;
    } while (horDoor <= 2 || horDoor >= (map_size - 2) ||
             m[y + topRoomHeight - 1][x + horDoor].tileType != gltactics::AIR ||
             m[y + topRoomHeight + 1][x + horDoor].tileType != gltactics::AIR);
    m[y + topRoomHeight][x + horDoor].tileType = gltactics::DOOR;
    m[y + topRoomHeight][x + horDoor].attributeType = gltactics::HORIZONTAL;
    doors.push_back({y + topRoomHeight, x + horDoor});
    sectors.push_back({.x=(size_t)(std::max(int(x) - 1, 0)), .y=(size_t)(std::max(int(y) - 1, 0)), .w=(width + 1), .h=(height + 1)});
    auto bottomRoomHeight = height - topRoomHeight;
    if (bottomRoomHeight * width <= 16 || topRoomHeight * width <= 16) return;
    verticalSplit(x, y, width, topRoomHeight, horDoor);
    verticalSplit(x, y + topRoomHeight, width, bottomRoomHeight, horDoor);
}

gltactics::map_generator::map_generator(std::mt19937_64 &generator) : generator{generator} {}

std::vector<gltactics::attribute> gltactics::map_generator::placeChests(bool onLeftSide) {
    std::uniform_int_distribution<size_t> dis(1, DEFAULT_MAPSIZE - 1);
    std::uniform_int_distribution<> chest_count_dis(1, 3);
    int chest_count = chest_count_dis(generator->get());
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
        int keyIndex = chest_count_dis(generator->get());
        this->chests[i] = std::optional<chest>(chest(keys[keyIndex]));
        std::array<size_t, 2> chestTile{};
        do {
            chestTile = {dis(generator->get()), xOffset + dis(generator->get()) / 2};
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

void gltactics::map_generator::placeDoors(const std::vector<gltactics::attribute> &doorTypes, bool onRightSide,
                                          size_t vSplit) {
    std::vector<std::array<size_t, 2>> doorsToLock{};
    std::uniform_int_distribution<size_t> dis(0, 50);
    int idx = 0;
    int xOffset = onRightSide ? vSplit + 1 : 1;
    int iterCount = 0;
    while (idx != doorTypes.size() && iterCount < 2) {
        for (size_t x = xOffset; x < xOffset + map_size - vSplit - 1; x++) {
            for (size_t y = 0; y < gltactics::map_generator::map_size; y++) {
                if (this->m[y][x].tileType == gltactics::DOOR && idx < doorTypes.size() && dis(generator->get()) > 40) {
                    doorsToLock.push_back({y, x});
                    idx++;
                }
            }
        }
        iterCount++;
    }
    for (int i = 0; i < doorsToLock.size(); i++) {
        auto[y, x] = doorsToLock[i];
        auto &door = this->m[y][x];
        door.attributeType = static_cast<gltactics::attribute>(door.attributeType | (gltactics::attribute) (
                doorTypes[i] | gltactics::LOCKED));
    }
}


gltactics::map<gltactics::DEFAULT_MAPSIZE> gltactics::map_generator::buildMap() {
    initialize();
    auto vSplit = verticalSplit(0, 0, map_size, map_size);
    auto doorTypes = placeChests(hFlip);
    placeDoors(doorTypes, hFlip, vSplit);
    placeExit(hFlip);
    gltactics::map<map_size> generatedMap = copyMap();
    for (int i = 0; i < 8; i++) generatedMap.setChest(i, chests[i] ? *(chests[i]) : std::optional<chest>());
    auto size = doors.size();
    std::list<std::array<size_t, 2>> tmpDoors;
    std::list<room_def> tmpSectors;
    for (int i = 0; i < size; i++) {
        std::array<size_t, 2> &point = doors.front();
        std::array<size_t, 2> door = {(point[1]), (point[0])};
        room_def &sector = sectors.front();
        generatedMap.addSection(sector, door);
        tmpDoors.push_front(doors.front());
        doors.pop_front();
        tmpSectors.push_front(sector);
        sectors.pop_front();
    }
    hFlip = !hFlip;
    mapList.push_back(generatedMap);
    std::ofstream file("map_data");
    file.flush();
    file.close();
    return generatedMap;
}

gltactics::map<gltactics::map_generator::map_size> gltactics::map_generator::copyMap() const {
    gltactics::map<gltactics::map_generator::map_size> generatedMap{0};
    for (size_t y = 0; y < gltactics::map_generator::map_size; y++) {
        for (size_t x = 0; x < gltactics::map_generator::map_size; x++) {
            generatedMap[{y, x}] = this->m[y][x];
        }
    }
    return generatedMap;
}

void gltactics::map_generator::initialize() {
    std::fill(std::begin(this->chests), std::end(this->chests), std::optional<chest>());
    std::fill(&this->m[0][0],
              &this->m[gltactics::map_generator::map_size - 1][gltactics::map_generator::map_size - 1] + 1, 0);
    for (int i = 0; i < gltactics::map_generator::map_size; i++) {
        this->m[0][i].tileType = gltactics::WALL;
        this->m[i][0].tileType = gltactics::WALL;
        this->m[gltactics::map_generator::map_size - 1][i].tileType = gltactics::WALL;
        this->m[i][gltactics::map_generator::map_size - 1].tileType = gltactics::WALL;
    }
    this->doors = {};
}

void gltactics::map_generator::placeExit(bool onRightSide) {
    std::uniform_int_distribution<size_t> dis(0, map_size / 2);
    size_t xOffset = onRightSide ? map_size / 2 + 1 : 1;
    size_t x, y;
    do {
        x = dis(generator->get()) + xOffset;
        y = dis(generator->get()) * 2;
    } while (m[y][x].tileType != AIR);
    this->lastExit = {(int64_t) y, (int64_t) x};
    m[y][x].tileType = gltactics::type::EXIT;
}

int gltactics::map_generator::currentFloor() {
    return mapList.size();
}

gltactics::map_generator::map_generator(std::nullptr_t) {
    this->generator = std::optional<std::reference_wrapper<std::mt19937_64>>();
}

gltactics::map_generator::map_generator(const gltactics::map_generator &newGenerator) {
    this->generator = newGenerator.generator;
}
