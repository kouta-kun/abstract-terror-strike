//
// Created by Bedroom1 on 31/05/2020.
//

#ifndef ABSTRACT_TERROR_STRIKE_RENDERING_HPP
#define ABSTRACT_TERROR_STRIKE_RENDERING_HPP
#include "raylib.h"
namespace gltactics {
    class tile;
}
void RenderStair(Vector3 stairPosition, const gltactics::tile &tile);

void RenderChest(Vector3 chestPosition, const gltactics::tile &tile);

void RenderDoor(Vector3 doorPosition, const gltactics::tile &tile);

void RenderWall(Vector3 wallPosition, const gltactics::tile &tile);
#endif //ABSTRACT_TERROR_STRIKE_RENDERING_HPP
