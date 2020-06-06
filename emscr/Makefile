raylib_dir = /root/raylib/build/src

cxxflags = -std=c++20 -O1 -Os -g -s WASM=0
ldflags = $(raylib_dir)/libraylib.bc -s USE_GLFW=3 -s ASYNCIFY=1

ats.html: compiled_shell.html character.o chest.o gen_icons.o image_cache.o main.o map.o map_generator.o tile.o game_manager.o rendering.o ghost.o
	em++ --shell-file $^ -o $@ $(ldflags) $(cxxflags)

%.o: ../%.cpp
	em++ $^ -c -o $@ -I /root/raylib/src $(cxxflags)

compiled_shell.html: shell.html
	bash compile_shell.sh

.PHONY: clean

clean:
	rm -f *.o compiled_shell.html ats.*
