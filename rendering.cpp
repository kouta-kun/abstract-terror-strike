//
// Created by Bedroom1 on 31/05/2020.
//
#include "rendering.hpp"
#include "map.hpp"
#include "game_manager.h"
#include "image_cache.h"

void RenderStair(Vector3 stairPosition, const gltactics::tile &tile) {
    DrawCube(stairPosition, 1.f, 0.25f, 1.f, DARKGRAY);
    stairPosition.y += 0.25f;
    stairPosition.x += 0.25f;
    DrawCube(stairPosition, 0.75f, 0.25f, 1.f, GRAY);
    stairPosition.y += 0.25f;
    stairPosition.x += 0.25f;
    DrawCube(stairPosition, 0.5f, 0.25f, 1.f, LIGHTGRAY);
}

void RenderChest(Vector3 chestPosition, const gltactics::tile &tile) {
    DrawCube(chestPosition, 1.0f, 1.0f, 1.0f, YELLOW);
}

void RenderDoor(Vector3 doorPosition, const gltactics::tile &tile) {
    Color doorColor;
    if (!tile.isLocked()) doorColor = ORANGE;
    else if (tile.isRed()) doorColor = RED;
    else if (tile.isGreen()) doorColor = GREEN;
    else if (tile.isBlue()) doorColor = BLUE;
    else return;
    if (tile.isHorizontal()) {
        if (tile.isOpen()) {
            doorPosition.x -= 0.25;
            DrawCube(doorPosition, 0.5f, 1.0f, 1.0f, doorColor);
        } else {
            DrawCube(doorPosition, 1.0f, 1.0f, 0.5f, doorColor);
        }
    } else {
        if (tile.isOpen()) {
            doorPosition.z -= 0.25;
            DrawCube(doorPosition, 1.0f, 1.0f, 0.5f, doorColor);
        } else {
            DrawCube(doorPosition, 0.5f, 1.0f, 1.0f, doorColor);
        }
    }
}

void RenderWall(Vector3 wallPosition, const gltactics::tile &tile) {
    DrawCube(wallPosition, 1.0f, 1.0f, 1.0f, DARKGRAY);
}

#define TRANSLATE_CAMERA(axis, amount) camera.position.axis += (amount); camera.target.axis += (amount)

constexpr int quantization = 6;

Vector2 center(Rectangle dist) {
    auto x = dist.x + dist.width / 2.0f;
    auto y = dist.y + dist.height / 2.0f;
    return Vector2{x, y};
}

float distance(Vector2 a, Vector2 b) {
    auto x = fabs(a.x - b.x);
    auto y = fabs(a.y - b.y);
    return (x * x) + (y * y);
}

void gltactics::game_manager::renderGameState() {
    // Update Camera
    cameraPositionChunk();

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(WHITE);

    DrawRectangle(0, 0, screenWidth, screenHeight, LIGHTGRAY);

    BeginMode3D(camera);

    drawFloor();
    drawPlayer();
    drawGhost();

    for (size_t y = 0; y < gltactics::DEFAULT_MAPSIZE; y++) {
        for (size_t x = 0; x < gltactics::DEFAULT_MAPSIZE; x++) {
            Vector3 wallPosition = {static_cast<float>(x), 0.0f, static_cast<float>(y)};
            Vector2 tileRoom = {x, y};
            if ((distance(_playerCharacter.position(), tileRoom) / 4) < 24.0f) {
                gltactics::tile &tile = currentMap[{y, x}];
                renderTile(wallPosition, tile);
            }
        }
    }
    EndMode3D();

    drawBlackOverlay();

    drawHud();

    EndDrawing();
}

void gltactics::game_manager::drawBlackOverlay() {
    const Vector2 &pixelChar = GetWorldToScreen(this->_playerCharacter.position3D(), this->camera);
    auto yMax = std::max(int(pixelChar.y - 125), 0);
    DrawRectangle(0, 0, gltactics::game_manager::screenWidth, yMax, BLACK);
    auto yMin = std::min(int(pixelChar.y + 125), int(gltactics::game_manager::screenHeight));
    DrawRectangle(0, yMin, gltactics::game_manager::screenWidth, gltactics::game_manager::screenHeight - yMin, BLACK);
    auto xMax = std::max(int(pixelChar.x - 125), 0);
    DrawRectangle(0, 0, xMax, gltactics::game_manager::screenWidth, BLACK);
    auto xMin = std::min(int(pixelChar.x + 125), int(gltactics::game_manager::screenWidth));
    DrawRectangle(xMin, 0, gltactics::game_manager::screenWidth - xMin, gltactics::game_manager::screenHeight, BLACK);
    for (size_t y = yMax; y < yMin; y++) {
        for (size_t x = xMax; x < xMin; x++) {
            if (sqrt(distance({x, y}, pixelChar)) > 125) {
                DrawPixel(x, y, BLACK);
            }
        }
    }
}

void gltactics::game_manager::drawHud() {
    DrawRectangle(0, 0, screenWidth, 48, BLACK);
    DrawRectangle(0, screenHeight - 90, screenWidth, 90, BLACK);
    gltactics::overMapRange(this->_playerCharacter,
                            [](size_t x, size_t y, gltactics::map<> &mapRef, bool &exitLoop) {
                                if (mapRef[{y, x}].tileType == gltactics::DOOR) {
                                    DrawText("F to open/close doors", 0, screenHeight - 90, 22, WHITE);
                                    exitLoop = true;
                                }
                            }); // label to open doors

    overMapRange(this->_playerCharacter,
                 [](size_t x, size_t y, gltactics::map<> &mapRef, bool &exitLoop) {
                     if (mapRef[{y, x}].tileType == gltactics::CHEST) {
                         DrawText("F to open chests", 0, screenHeight - 90 + 24, 22, WHITE);
                         exitLoop = true;
                     }
                 }); // label to open chests
    const std::set<int> &inventory = this->_playerCharacter.getInventoryList();
    if (!inventory.empty()) { // label to use items
        DrawText("E to use items", 0, screenHeight - 90 + 48, 22, WHITE);
    }
    int i = 0;
    for (int const &item : inventory) {
        if ((unsigned int) item & 0x10u) {
            i++;
            unsigned int color_index = ((unsigned int) item & 0x0Fu);
            Color color = gltactics::keyColors[color_index];
            auto &icn_texture = gltactics::image_cache::getCache()(gltactics::make_key_icon, color, 32, 32);
            DrawTexture(icn_texture, 8 + (32 + 8) * i, 8, WHITE); // draw items
        }
    }
    auto floorLabel = TextFormat("Current floor: %d", this->mapGenerator.currentFloor());
    int rightMargin = MeasureText(floorLabel, 22) + 2;
    DrawText(floorLabel, screenWidth - rightMargin, screenHeight - 90, 22, WHITE);
}

void gltactics::game_manager::renderTile(const Vector3 &tilePosition, const gltactics::tile &tile) {
    switch (tile.tileType) {
        case gltactics::WALL:
            RenderWall(tilePosition, tile);
            break;

        case gltactics::DOOR:
            RenderDoor(tilePosition, tile);
            break;

        case gltactics::CHEST:
            RenderChest(tilePosition, tile);
            break;

        case gltactics::EXIT:
            RenderStair(tilePosition, tile);
            break;

        default:
            break;
    }
}

void gltactics::game_manager::drawPlayer() {
    Vector3 playerPosition = this->_playerCharacter.position3D();
    DrawCube(playerPosition, 1.0f, 1.0f, 1.0f, this->_playerCharacter.color());
}

void gltactics::game_manager::drawFloor() {
    Vector3 floorCenter = {((float) gltactics::DEFAULT_MAPSIZE - 1.0f) / 2.0f, 0.05f,
                           ((float) gltactics::DEFAULT_MAPSIZE - 1.0f) / 2.0f};
    DrawCube(floorCenter, gltactics::DEFAULT_MAPSIZE - 2.0f, 0.1f, gltactics::DEFAULT_MAPSIZE - 2.0f, RAYWHITE);
}

void gltactics::game_manager::cameraPositionChunk() {// Update Camera
//----------------------------------------------------------------------------------
    std::array<float, 2> cameraChunks = this->calculateCameraChunk();

    auto[yChunk, xChunk] = cameraChunks;

    float xDistance = (xChunk - this->camera.position.x);
    float yDistance = (yChunk - this->camera.position.z);
    if (std::abs(xDistance) > 0.2f || std::abs(yDistance) > 0.2f) {
        float xTr = xDistance / (10.f / 3.f);
        TRANSLATE_CAMERA(x, xTr);
        float yTr = yDistance / (10.f / 3.f);
        TRANSLATE_CAMERA(z, yTr);
    }
    UpdateCamera(&this->camera);
}

std::array<float, 2> gltactics::game_manager::calculateCameraChunk() const {
    float xChunk = (float) ((int) (this->_playerCharacter.position3D().x / quantization)) * quantization;
    float yChunk = (float) ((int) (this->_playerCharacter.position3D().z / quantization)) * quantization;
    return {yChunk, xChunk};
}

void gltactics::game_manager::drawGhost() {
    if ((distance(_playerCharacter.position(), _ghost.position()) / 4) < 24.0f) {
        Vector3 playerPosition = this->_ghost.position3D();
        DrawCube(playerPosition, 1.0f, 1.0f, 1.0f, this->_ghost.color());
    }
}
