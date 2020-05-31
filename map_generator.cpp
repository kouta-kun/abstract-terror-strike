//
// Created by Bedroom1 on 19/05/2020.
//

#include "map_generator.hpp"
#include "character.hpp"

void gltactics::map_generator::vertical_split(ssize_t x, ssize_t y, ssize_t width, ssize_t height, ssize_t hor_door) {
    if (height <= 6) return;
    ssize_t left_room_width; // separes room into left and right rooms
    ssize_t w_rand_mod = 2;
    std::uniform_int_distribution<> w_dis(-w_rand_mod, w_rand_mod);
    ssize_t h_rand_mod = (height - 1);
    std::uniform_int_distribution<> h_dis(0, h_rand_mod);
    do {
        left_room_width = (width / 2) + (w_dis(generator));
    } while (hor_door >= 0 && left_room_width == hor_door);
    ssize_t vert_door;
    int itercount = 1;
    do {
        vert_door = h_dis(generator); // door on the line between both rooms
        if(itercount++ > 15) {
            return;
        }
    } while (vert_door <= 2 || vert_door >= (map_size - 2) ||
             m[y + vert_door][x + left_room_width - 1].tileType != gltactics::AIR ||
             m[y + vert_door][x + left_room_width + 1].tileType != gltactics::AIR);
    for (ssize_t y2 = 0; y2 < height; y2++) {
        m[y + y2][x + left_room_width].tileType = gltactics::WALL;
    }
    m[y + vert_door][x + left_room_width].tileType = gltactics::DOOR;
    ssize_t right_room_width = width - left_room_width;
    if (left_room_width * height > 16 && right_room_width * height > 16) {
        horizontal_split(x, y, left_room_width, height, vert_door);
        horizontal_split(left_room_width + x, y, right_room_width, height, vert_door);
    }
}

void
gltactics::map_generator::horizontal_split(ssize_t x, ssize_t y, ssize_t width, ssize_t height, ssize_t vert_door) {
    if (width <= 6) return;
    ssize_t top_room_height; // separates room into top and bottom rooms
    ssize_t w_rand_mod = width - 1;
    std::uniform_int_distribution<> w_dis(1, w_rand_mod);
    ssize_t h_rand_mod = 2;
    std::uniform_int_distribution<> h_dis(-h_rand_mod, h_rand_mod);
    do {
        top_room_height = (height / 2) + (h_dis(generator));
    } while (vert_door >= 0 && top_room_height == vert_door);
    for (ssize_t x2 = 0; x2 < width; x2++) {
        m[y + top_room_height][x + x2].tileType = gltactics::WALL;
    }
    ssize_t hor_door;
    do {
        hor_door = w_dis(generator);
    } while (hor_door <= 2 || hor_door >= (map_size - 2) ||
             m[y + top_room_height - 1][x + hor_door].tileType != gltactics::AIR ||
             m[y + top_room_height + 1][x + hor_door].tileType != gltactics::AIR);
    m[y + top_room_height][x + hor_door].tileType = gltactics::DOOR;
    m[y + top_room_height][x + hor_door].attributeType = gltactics::HORIZONTAL;
    ssize_t bottom_room_height = height - top_room_height;
    if (bottom_room_height * width > 16 && top_room_height * width > 16) {
        vertical_split(x, y, width, top_room_height, hor_door);
        vertical_split(x, y + top_room_height, width, bottom_room_height, hor_door);
    }
}

gltactics::map_generator::map_generator(int seed) : generator(seed) {

}

std::vector<gltactics::attribute> gltactics::map_generator::place_chests() {
    std::uniform_int_distribution<> dis(0, DEFAULT_MAPSIZE);
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

    for (int i = 0; i < chest_count; i++) {
        int keyIndex = chest_count_dis(generator);
        this->chests[i] = new chest(keys[keyIndex]);
        std::array<ssize_t, 2> chestTile{};
        do {
            chestTile = {dis(generator), dis(generator) / 2};
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

void gltactics::map_generator::placeDoors(const std::vector<gltactics::attribute> &doorTypes) {
    std::vector<std::array<ssize_t, 2>> doorsToLock{};
    std::uniform_int_distribution<> dis(50);
    int idx = 0;
    while (idx != doorTypes.size()) {
        for (int x = gltactics::map_generator::map_size / 2 + 1; x < gltactics::map_generator::map_size; x++) {
            for (int y = 0; y < gltactics::map_generator::map_size; y++) {
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


gltactics::map<gltactics::DEFAULT_MAPSIZE> gltactics::map_generator::build_map() {
    for (int i = 0; i < map_size; i++) {
        m[0][i].tileType = gltactics::WALL;
        m[i][0].tileType = gltactics::WALL;
        m[map_size - 1][i].tileType = gltactics::WALL;
        m[i][map_size - 1].tileType = gltactics::WALL;
    }
    vertical_split(0, 0, map_size, map_size);
    auto doorTypes = place_chests();
    placeDoors(doorTypes);
    gltactics::map<map_size> generatedMap{0};
    for (int y = 0; y < map_size; y++) {
        for (int x = 0; x < map_size; x++) {
            generatedMap[{y, x}] = m[y][x];
        }
    }
    for (int i = 0; i < 8; i++) generatedMap.setChest(i, chests[i]);
    return generatedMap;
}
