cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(abstract_terror_strike)
if(${PLATFORM} STREQUAL "Web")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -s USE_GLFW=3 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
  # Since WASM is used, ALLOW_MEMORY_GROWTH has no extra overheads
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s ALLOW_MEMORY_GROWTH=1 --no-heap-copy")
  set(OUTPUT_EXT ".html")
endif()
# Set this to the minimal version you want to support
find_package(raylib 3.0 QUIET) # Let CMake search for a raylib-config.cmake

# You could change the QUIET above to REQUIRED and remove this if() clause
# This part downloads raylib and builds it if it's not installed on your system
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)

    FetchContent_Declare(
            raylib
            URL https://github.com/raysan5/raylib/archive/master.tar.gz
    )

    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)

        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples

        # build raylib
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(abstract_terror_strike
        constants.hpp
        chest.hpp character.hpp
        map.hpp map_generator.hpp
        tile.cpp image_cache.h
        chest.cpp character.cpp
        map.cpp map_generator.cpp game_manager.cpp game_manager.h
        main.cpp rendering.cpp rendering.hpp
        gen_icons.cpp gen_icons.h image_cache.cpp game_manager.cpp game_manager.h)

target_link_libraries(abstract_terror_strike raylib glfw)
