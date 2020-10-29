//
// Created by kouta on 24/7/20.
//

#ifndef ABSTRACT_TERROR_STRIKE_STDIO_PLATFORM_H
#define ABSTRACT_TERROR_STRIKE_STDIO_PLATFORM_H


#include "platform_module.h"
#include <game_manager.h>
namespace gltactics {
    class stdio_platform : public gltactics::platform_module {
        gltactics::game_manager &manager;
    public:
        stdio_platform(gltactics::game_manager &gm);

        virtual ~stdio_platform() override = default;

        void initialize() override;

        void render() override;

        input_status get_input() override;
    };
}

#endif //ABSTRACT_TERROR_STRIKE_STDIO_PLATFORM_H
