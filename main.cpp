#include "raylib.h"
#include <array>
#include <fstream>
#include <ctime>
#include "map.hpp"
#include "character.hpp"
#include "map_generator.hpp"

#define TRANSLATE_CAMERA(axis, amount) camera.position.axis += (amount); camera.target.axis += (amount)
namespace gltactics {
    typedef void (*rangeFunction)(ssize_t x, ssize_t y, map<> &object, bool *exitLoop);

    void overMapRange(gltactics::character<> &playerCharacter, rangeFunction mapFunction) {
        bool exitLoop = false;
        for (ssize_t x = std::max((ssize_t) 0, (ssize_t) playerCharacter.position().x - 2);
             x < std::min(DEFAULT_MAPSIZE, (ssize_t) playerCharacter.position().x + 2); x++) {
            for (ssize_t y = std::max((ssize_t) 0, (ssize_t) playerCharacter.position().y - 2);
                 y < std::min(DEFAULT_MAPSIZE, (ssize_t) playerCharacter.position().y + 2); y++) {
                mapFunction(x, y, playerCharacter.map(), &exitLoop);
                if (exitLoop) return;
            }
        }
    }
}

int main() {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 640;
    const int screenHeight = 480;

    srand(time(nullptr));
    gltactics::map<> myMap = gltactics::map_generator(rand()).build_map();

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
        else if (IsKeyPressed(KEY_F)) {
            overMapRange(playerCharacter,
                         [](ssize_t x, ssize_t y, gltactics::map<> &mapRef, bool *_) {
                             gltactics::type tileType = mapRef[{y, x}].tileType;
                             if (tileType == gltactics::type::DOOR)
                                 mapRef[{y, x}].attributeType = (gltactics::attribute) (mapRef[{y, x}].attributeType ^
                                                                                        gltactics::attribute::OPEN);
                         });
        }

        // Update Camera
        //----------------------------------------------------------------------------------
        auto quantization = 6;
        float xChunk = (float)((int) playerCharacter.position3d().x / quantization) * quantization;
        float xDistance = (xChunk - camera.position.x);
        float yChunk = (float)((int) playerCharacter.position3d().z / quantization) * quantization;
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

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        Vector3 playerPosition = playerCharacter.position3d();
        DrawCube(playerPosition, 1.0f, 1.0f, 1.0f, playerCharacter.color());
        for (int y = 0; y < gltactics::DEFAULT_MAPSIZE; y++) {
            for (int x = 0; x < gltactics::DEFAULT_MAPSIZE; x++) {
                Vector3 wallPosition = {static_cast<float>(x), 0.0f, static_cast<float>(y)};
                switch (myMap[{y, x}].tileType) {
                    case gltactics::type::WALL:
                        DrawCube(wallPosition, 1.0f, 1.0f, 1.0f, RED);
                        break;

                    case gltactics::type::DOOR:
                        if (myMap[{y, x}].isHorizontal()) {
                            if (myMap[{y, x}].isOpen()) {
                                wallPosition.x -= 0.25;
                                DrawCube(wallPosition, 0.5f, 1.0f, 1.0f, ORANGE);
                            } else {
                                DrawCube(wallPosition, 1.0f, 1.0f, 0.5f, ORANGE);
                            }
                        } else {
                            if (myMap[{y, x}].isOpen()) {
                                wallPosition.z -= 0.25;
                                DrawCube(wallPosition, 1.0f, 1.0f, 0.5f, ORANGE);
                            } else {
                                DrawCube(wallPosition, 0.5f, 1.0f, 1.0f, ORANGE);
                            }
                        }
                    default:
                        break;
                }
            }
        }

        EndMode3D();

        overMapRange(playerCharacter,
                     [](ssize_t x, ssize_t y, gltactics::map<> &mapRef, bool *exitLoop) {
                         if (mapRef[{y, x}].tileType == gltactics::type::DOOR) {
                             DrawText("F to open/close doors", 0, 400, 22, BLACK);
                             *exitLoop = true;
                         }
                     });
        EndDrawing();
    }

    // De-Initialization
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
