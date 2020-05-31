//
// Created by Bedroom1 on 30/05/2020.
//

#include "image_cache.h"
#include "gen_icons.h"

std::shared_ptr<gltactics::image_cache> gltactics::image_cache::instance = nullptr;

bool operator==(const Color a, const Color b) {
    auto &aa = a;
    auto &bb = b;
    return reinterpret_cast<const uint32_t&>(a) == reinterpret_cast<const uint32_t&>(b);
}

Texture2D &gltactics::image_cache::operator()(iconFunction function, Color color, int w, int h) {
    const std::tuple<iconFunction, Color, int, int> index = std::make_tuple(function, color, w, h);
    if(!this->textureCache.contains(index)) {
        auto image = ImageCopy(operator()(function, color));
        ImageResizeNN(&image, w, h);
        textureCache[index] = LoadTextureFromImage(image);
    }
    return textureCache[index];
}

Image &gltactics::image_cache::operator()(iconFunction function, Color color) {
    const std::tuple<iconFunction, Color> index = std::make_tuple(function, color);
    if(!this->imageCache.contains(index)) {
        imageCache[index] = gltactics::iconToImage(operator()(function), color);
    }
    return imageCache[index];
}

gltactics::cmpIcon &gltactics::image_cache::operator()(gltactics::iconFunction index) {
    if(!this->iconCache.contains(index)) {
        iconCache[index] = index();
    }
    return iconCache[index];
}

gltactics::image_cache &gltactics::image_cache::getCache() {
    if(gltactics::image_cache::instance == nullptr) {
        instance = std::shared_ptr<gltactics::image_cache>(new gltactics::image_cache());
    }
    return *instance;
}

gltactics::image_cache::~image_cache() {
    for(auto tex : textureCache) {
        UnloadTexture(tex.second);
    }
    for(auto img : imageCache) {
        UnloadImage(img.second);
    }
}
