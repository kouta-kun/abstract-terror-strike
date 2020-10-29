//
// Created by kouta on 24/7/20.
//

#ifndef ABSTRACT_TERROR_STRIKE_POVRAY_PLATFORM_H
#define ABSTRACT_TERROR_STRIKE_POVRAY_PLATFORM_H


#include <SDL2/SDL_video.h>
#include "platform_module.h"
#include "game_manager.h"

namespace gltactics {
    class povray_platform : public gltactics::platform_module {
        gltactics::game_manager &gameManager;
        SDL_Window *window;
        SDL_Surface *winSurface;
        size_t frameN;
    public:
        povray_platform(game_manager &gameManager);

        void initialize() override;

        void render() override;

        input_status get_input() override;
    };
};

#endif //ABSTRACT_TERROR_STRIKE_POVRAY_PLATFORM_H
