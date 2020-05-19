#include "raylib.h"
#include <cstdint>
#include <array>
#include <fstream>
#include <cmath>
#include "map.hpp"
#include "character.hpp"

#define translateCamera(axis, amount) {camera.position.axis += amount; camera.target.axis += amount;}
namespace gltactics {
  typedef void (*RangeFunction)(size_t x, size_t y, map<>& object, bool &exitLoop);

  void OverMapRange(gltactics::character<> &playerCharacter, RangeFunction map_function) {
    bool exitLoop;
    for(size_t x = std::max((ssize_t)0, (ssize_t)playerCharacter.position().x - 2); x < std::min(DEFAULT_MAPSIZE, (ssize_t)playerCharacter.position().x + 2); x++) {
      for(size_t y = std::max((ssize_t)0, (ssize_t)playerCharacter.position().y - 2); y < std::min(DEFAULT_MAPSIZE, (ssize_t)playerCharacter.position().y + 2); y++) {
	map_function(x,y, playerCharacter.map(), exitLoop);
	if(exitLoop) return;
      }
    }
  };
};

int main(void)
{
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 640;
  const int screenHeight = 480;

  gltactics::map<> myMap = gltactics::map_generator<>().build_map(336);

  InitWindow(screenWidth, screenHeight, "gltactics");

  // Define the camera to look into our 3d world
  Camera3D camera = { 0 };
  camera.position = (Vector3){ 0.0f, 10.0f, 0.0f };    // Camera position
  camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };       // Camera looking at point
  camera.up = (Vector3){ 0.0f, 0.0f, -1.0f };          // Camera up vector (rotation towards target)
  camera.fovy = 45.0f;                                 // Camera field-of-view Y
  camera.type = CAMERA_PERSPECTIVE;                    // Camera mode type

  SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  gltactics::character<> playerCharacter (BLUE, (Vector2){1,1}, myMap);
  // Main game loop
  while (!WindowShouldClose()) {      // Detect window close button or ESC key
    // input handling
    if(IsKeyPressed(KEY_LEFT))
      playerCharacter.move(gltactics::direction::left);
    else if(IsKeyPressed(KEY_RIGHT))
      playerCharacter.move(gltactics::direction::right);
    else if(IsKeyPressed(KEY_DOWN))
      playerCharacter.move(gltactics::direction::down);
    else if(IsKeyPressed(KEY_UP))
      playerCharacter.move(gltactics::direction::up);
    else if(IsKeyPressed(KEY_F)) {
      OverMapRange(playerCharacter,
		   [](size_t x, size_t y, gltactics::map<> &mapRef, bool&_) {
		     if(mapRef[{y,x}].tileType == gltactics::type::DOOR)
		       mapRef[{y,x}].attributeType = (gltactics::attribute)(mapRef[{y,x}].attributeType ^ gltactics::attribute::OPEN);
		   });
    }

    // Update Camera
    //----------------------------------------------------------------------------------
    float x_distance = (playerCharacter.position3d().x - camera.position.x);
    float y_distance = (playerCharacter.position3d().z - camera.position.z);
    if(std::abs(x_distance) > 0.2f || std::abs(y_distance) > 0.2f) {
      translateCamera(x, (x_distance/2.0f));
      translateCamera(z, (y_distance/2.0f));
    }
    UpdateCamera(&camera);

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    
    ClearBackground(RAYWHITE);
    
    BeginMode3D(camera);
    
    Vector3 playerPosition = playerCharacter.position3d();
    DrawCube(playerPosition, 1.0f, 1.0f, 1.0f, playerCharacter.color());
    for(int y = 0; y < gltactics::DEFAULT_MAPSIZE; y++) {
      for(int x = 0; x < gltactics::DEFAULT_MAPSIZE; x++) {
	Vector3 wallPosition = {x, 0.0f, y};
	switch(myMap[{y,x}].tileType) {
	case gltactics::type::WALL:
	  DrawCube(wallPosition, 1.0f, 1.0f, 1.0f, RED);
	  break;

	case gltactics::type::DOOR:
	  bool isHorizontal = myMap[{y,x}].attributeType & gltactics::attribute::HORIZONTAL;
	  bool isOpen = myMap[{y,x}].attributeType & gltactics::attribute::OPEN;
	  if(isHorizontal){
	    if(isOpen) {
	      wallPosition.x -= 0.25;
	      DrawCube(wallPosition, 0.5f, 1.0f, 1.0f, ORANGE);
	    } else {
	      DrawCube(wallPosition, 1.0f, 1.0f, 0.5f, ORANGE);
	    }
	  } else {
	    if(isOpen){
	      wallPosition.z -= 0.25;
	      DrawCube(wallPosition, 1.0f, 1.0f, 0.5f, ORANGE);
	    } else {
	      DrawCube(wallPosition, 0.5f, 1.0f, 1.0f, ORANGE);
	    }
	  }
	}
      }
    }
    
    EndMode3D();

    OverMapRange(playerCharacter,
		 [](size_t x, size_t y, gltactics::map<> &mapRef, bool &exitLoop) {
		   if(mapRef[{y,x}].tileType == gltactics::type::DOOR) {
		     DrawText("F to open/close doors", 0, 400, 22,BLACK);
		     exitLoop = true;
		   }
		 });    
    EndDrawing();
  }
  
  // De-Initialization
  CloseWindow();        // Close window and OpenGL context

  return 0;
}
