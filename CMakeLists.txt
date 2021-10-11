cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(abstract_terror_strike)
if(${PLATFORM} STREQUAL "Web")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -s USE_GLFW=3 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
  # Since WASM is used, ALLOW_MEMORY_GROWTH has no extra overheads
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s ALLOW_MEMORY_GROWTH=1 --no-heap-copy")
  set(OUTPUT_EXT ".html")
endif()

set(CMAKE_CXX_STANDARD 20)

find_package(fmt)
find_package(SDL2)

include_directories(.)

add_executable(abstract_terror_strike
        constants.hpp
        chest.hpp character.hpp
        map.hpp map_generator.hpp
        tile.cpp image_cache.h
        chest.cpp character.cpp
        map.cpp map_generator.cpp
        main.cpp
        gen_icons.cpp gen_icons.h
        game_manager.cpp game_manager.h
        ghost.cpp ghost.h platform_module.h
        stdio_platform.cpp stdio_platform.h
        glfw_platform.cpp glfw_platform.h
        glfwInitialization.cpp glfwInitialization.h
        )

target_link_libraries(abstract_terror_strike fmt glfw GL GLEW assimp)