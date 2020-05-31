//
// Created by Bedroom1 on 30/05/2020.
//

#include <vector>
#include <tuple>
#include "raylib.h"

#ifndef ABSTRACT_TERROR_STRIKE_GEN_ICONS_H
namespace gltactics {
    struct cmpIcon {
        ssize_t width;
        ssize_t height;
        std::vector<bool> image;

        cmpIcon(ssize_t width, ssize_t height, std::vector<bool> image);
        cmpIcon() = default;
    };

    cmpIcon make_key_icon();

    Image iconToImage(const cmpIcon& icon, Color color);
};
#define ABSTRACT_TERROR_STRIKE_GEN_ICONS_H

#endif //ABSTRACT_TERROR_STRIKE_GEN_ICONS_H
