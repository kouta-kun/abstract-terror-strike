//
// Created by Bedroom1 on 30/05/2020.
//

#include <cstdint>
#include "gen_icons.h"

gltactics::cmpIcon::cmpIcon(const size_t width, const size_t height, std::vector<bool> image) : width(width),
                                                                                                  height(height),
                                                                                                  image(std::move(
                                                                                                          image)) {};

gltactics::cmpIcon gltactics::make_key_icon() {
    std::vector<bool> boolVec = {
            0, 0, 0, 0, 1, 1, 1, 1,
            0, 0, 0, 0, 1, 0, 0, 0,
            0, 0, 0, 0, 1, 1, 1, 1,
            0, 0, 0, 0, 1, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0,
            0, 0, 0, 1, 0, 1, 0, 0,
            0, 0, 1, 0, 0, 0, 1, 0,
            0, 0, 0, 1, 1, 1, 0, 0,
    };
    return cmpIcon(8,8,boolVec);
}

Image gltactics::iconToImage(const cmpIcon& icon, Color color) {
    Color image[icon.height * icon.width];
    for(int i = 0; i < icon.height * icon.width; i++) {
        image[i] = icon.image[i] ? color : BLANK;
    }
    return LoadImageEx(image, icon.width, icon.height);
}