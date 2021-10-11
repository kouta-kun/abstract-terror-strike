//
// Created by Bedroom1 on 30/05/2020.
//

#include <vector>
#include <tuple>
#include "raylib.h"
#include <cstdint>

#ifndef ABSTRACT_TERROR_STRIKE_GEN_ICONS_H
namespace gltactics {
    struct cmpIcon {
        int width;
        int height;
        std::vector<bool> image;

        cmpIcon(int width, int height, std::vector<bool> image);
        cmpIcon() = default;
    };

    cmpIcon make_key_icon();

    Image iconToImage(const cmpIcon& icon, Color color);
};
#define ABSTRACT_TERROR_STRIKE_GEN_ICONS_H

#endif //ABSTRACT_TERROR_STRIKE_GEN_ICONS_H
