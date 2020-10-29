//
// Created by kouta on 24/7/20.
//

#ifndef ABSTRACT_TERROR_STRIKE_PLATFORM_MODULE_H
#define ABSTRACT_TERROR_STRIKE_PLATFORM_MODULE_H

#include <optional>
#include "direction.hpp"


namespace gltactics {
    struct input_status {
        std::optional<gltactics::direction> movementDirection;
        bool items;
        bool environment;
    };

    class platform_module {
    public:
        virtual ~platform_module() = default;

        virtual void initialize() = 0;

        virtual void render() = 0;

        virtual input_status get_input() = 0;
    };
};

#endif //ABSTRACT_TERROR_STRIKE_PLATFORM_MODULE_H
