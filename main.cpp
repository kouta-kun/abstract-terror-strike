#include "raylib.h"
#include <fstream>
#include <ctime>
#include <iostream>
#include "constants.hpp"
#include "map.hpp"
#include "character.hpp"
#include "map_generator.hpp"
#include "image_cache.h"
#include "gen_icons.h"

#define TRANSLATE_CAMERA(axis, amount) camera.position.axis += (amount); camera.target.axis += (amount)
int main() {
    // Initialization
    //---------------
    const int screenWidth = 640;
    const int screenHeight = 480;

    srand(time(nullptr));
    int seed = rand();
    std::cout << "seed: " << seed << '\n';
    gltactics::map<> myMap = gltactics::map_generator(seed).build_map();

    InitWindow(screenWidth, screenHeight, "gltactics");

    // Define the camera to look into our 3d world
    Camera3D camera = {0};
    camera.position = (Vector3) {0.0f, 15.0f, 0.0f};    // Camera position
    camera.target = (Vector3) {0.0f, 0.0f, 0.0f};       // Camera looking at point
    camera.up = (Vector3) {0.0f, 0.0f, -1.0f};          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                 // Camera field-of-view Y
    camera.type = CAMERA_PERSPECTIVE;                    // Camera mode type

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    gltactics::character<> playerCharacter(BLUE, (Vector2) {1, 1}, myMap);
    // Main game loop
    while (!WindowShouldClose()) {      // Detect window close button or ESC key
        // input handling
        if (IsKeyPressed(KEY_LEFT))
            playerCharacter.move(gltactics::direction::left);
        else if (IsKeyPressed(KEY_RIGHT))
            playerCharacter.move(gltactics::direction::right);
        else if (IsKeyPressed(KEY_DOWN))
            playerCharacter.move(gltactics::direction::down);
        else if (IsKeyPressed(KEY_UP))
            playerCharacter.move(gltactics::direction::up);
        else if (IsKeyPressed(KEY_E))
            playerCharacter.useItems();
        else if (IsKeyPressed(KEY_F))
            playerCharacter.useEnvironment();

        // Update Camera
        //----------------------------------------------------------------------------------
        auto quantization = 6;
        float xChunk = (float) ((int) (playerCharacter.position3D().x / quantization)) * quantization;
        float xDistance = (xChunk - camera.position.x);
        float yChunk = (float) ((int) (playerCharacter.position3D().z / quantization)) * quantization;
        float yDistance = (yChunk - camera.position.z);
        if (std::abs(xDistance) > 0.2f || std::abs(yDistance) > 0.2f) {
            float xTr = xDistance / 2.0f;
            TRANSLATE_CAMERA(x, xTr);
            float yTr = yDistance / 2.0f;
            TRANSLATE_CAMERA(z, yTr);
        }
        UpdateCamera(&camera);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode3D(camera);
        Vector3 floorCenter = {((float) gltactics::DEFAULT_MAPSIZE - 1.0f) / 2.0f, 0.05f,
                               ((float) gltactics::DEFAULT_MAPSIZE - 1.0f) / 2.0f};
        DrawCube(floorCenter, gltactics::DEFAULT_MAPSIZE - 2.0f, 0.1f, gltactics::DEFAULT_MAPSIZE - 2.0f, RAYWHITE);

        Vector3 playerPosition = playerCharacter.position3D();
        DrawCube(playerPosition, 1.0f, 1.0f, 1.0f, playerCharacter.color());
        for (int y = 0; y < gltactics::DEFAULT_MAPSIZE; y++) {
            for (int x = 0; x < gltactics::DEFAULT_MAPSIZE; x++) {
                Vector3 wallPosition = {static_cast<float>(x), 0.0f, static_cast<float>(y)};
                gltactics::tile &tile = myMap[{y, x}];
                switch (tile.tileType) {
                    case gltactics::type::WALL:
                        DrawCube(wallPosition, 1.0f, 1.0f, 1.0f, DARKGRAY);
                        break;

                    case gltactics::type::DOOR: {
                        Color doorColor;
                        if (!tile.isLocked()) doorColor = ORANGE;
                        else if (tile.isRed()) doorColor = RED;
                        else if (tile.isGreen()) doorColor = GREEN;
                        else if (tile.isBlue()) doorColor = BLUE;
                        else break;
                        if (tile.isHorizontal()) {
                            if (tile.isOpen()) {
                                wallPosition.x -= 0.25;
                                DrawCube(wallPosition, 0.5f, 1.0f, 1.0f, doorColor);
                            } else {
                                DrawCube(wallPosition, 1.0f, 1.0f, 0.5f, doorColor);
                            }
                        } else {
                            if (tile.isOpen()) {
                                wallPosition.z -= 0.25;
                                DrawCube(wallPosition, 1.0f, 1.0f, 0.5f, doorColor);
                            } else {
                                DrawCube(wallPosition, 0.5f, 1.0f, 1.0f, doorColor);
                            }
                        }
                    }
                        break;

                    case gltactics::type::CHEST:
                        DrawCube(wallPosition, 1.0f, 1.0f, 1.0f, YELLOW);
                        break;
                    default:
                        break;
                }
            }
        }

        EndMode3D();

        overMapRange(playerCharacter,
                     [](ssize_t x, ssize_t y, gltactics::map<> &mapRef, bool &exitLoop) {
                         if (mapRef[{y, x}].tileType == gltactics::type::DOOR) {
                             DrawText("F to open/close doors", 0, 400, 22, WHITE);
                             exitLoop = true;
                         }
                     });

        overMapRange(playerCharacter,
                     [](ssize_t x, ssize_t y, gltactics::map<> &mapRef, bool &exitLoop) {
                         if (mapRef[{y, x}].tileType == gltactics::type::CHEST) {
                             DrawText("F to open chests", 0, 430, 22, WHITE);
                             exitLoop = true;
                         }
                     });
        const std::set<int> &inventory = playerCharacter.getInventoryList();
        if (!inventory.empty()) {
            DrawText("E to use items", 0, 460, 22, WHITE);
        }
        int i = 0;
        for (int const& item : inventory) {
            if ((unsigned int) item & 0x10u) {
                i++;
                unsigned int color_index = ((unsigned int)item & 0x0Fu);
                Color color = gltactics::keyColors[color_index];
                auto &icn_texture = gltactics::image_cache::getCache()(gltactics::make_key_icon, color, 32, 32);
                DrawTexture(icn_texture, 8 + (32 + 8) * i, 8, WHITE);
            }
        }
        EndDrawing();
    }

    // De-Initialization
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
