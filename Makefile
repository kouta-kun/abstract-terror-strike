CXXLIBS ?= -ggdb -O0


main.elf: main.o raylib/src/libraylib.a
	$(CXX) $(CXXLIBS) main.o -L raylib/src -lraylib -o main.exe -lWinMM -lopengl32 -lgdi32

main.o: main.cpp
	$(CXX) $(CXXLIBS) main.cpp -o main.o -c -I raylib/src

raylib/src/libraylib.a:
	mingw32-make -C raylib/src
