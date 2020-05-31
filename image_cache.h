//
// Created by Bedroom1 on 30/05/2020.
//

#ifndef ABSTRACT_TERROR_STRIKE_IMAGE_CACHE_H
#define ABSTRACT_TERROR_STRIKE_IMAGE_CACHE_H

#include <map>
#include <memory>
#include <functional>
#include "gen_icons.h"

namespace gltactics {
    typedef cmpIcon(*iconFunction)();
    struct hash_functionAndColor : public std::unary_function<std::tuple<iconFunction, Color>, std::size_t> {
        std::size_t operator()(const std::tuple<iconFunction, Color> &tup) const {
            Color color = std::get<1>(tup);
            std::uint32_t &colorInt = reinterpret_cast<uint32_t &>(color);
            return (ssize_t)std::get<0>(tup) ^ colorInt;
        }
    };

    struct hash_functionColorAndSize : public std::unary_function<std::tuple<iconFunction, Color, int, int>, std::size_t> {
        std::size_t operator()(const std::tuple<iconFunction, Color, int, int> &tup) const {
            auto &[fun, color, w, h] = tup;
            std::uint32_t const& colorInt = reinterpret_cast<uint32_t const&>(color);
            return (size_t)fun ^ colorInt ^ ((long long)w << 32LL) ^ h;
        }
    };


    class image_cache {
        static std::shared_ptr<image_cache> instance;
        std::unordered_map<iconFunction, cmpIcon> iconCache;
        std::unordered_map<std::tuple<iconFunction, Color>, Image, hash_functionAndColor> imageCache;
        std::unordered_map<std::tuple<iconFunction, Color, int, int>, Texture2D, hash_functionColorAndSize> textureCache;
        image_cache() = default;
    public:
        ~image_cache();
        Texture2D& operator()(iconFunction, Color, int, int);
        Image& operator()(iconFunction, Color);
        cmpIcon& operator()(iconFunction);
        static image_cache& getCache();
    };
};
#endif
//ABSTRACT_TERROR_STRIKE_IMAGE_CACHE_H
