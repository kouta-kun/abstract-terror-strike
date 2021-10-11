//
// Created by Bedroom1 on 30/05/2020.
//

#include <cstdint>
#include "gen_icons.h"

gltactics::cmpIcon::cmpIcon(const int width, const int height, std::vector<bool> image) : width(width),
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
    Color *image = new Color[icon.height * icon.width * sizeof(color)];
    for(int i = 0; i < icon.height * icon.width; i++) {
        image[i] = icon.image[i] ? color : BLANK;
    }
    return Image {
        .data = image,
        .width = icon.width,
        .height = icon.height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };
//LoadImageEx(image, icon.width, icon.height);
}
