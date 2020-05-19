#ifndef __MAP_HEADER
#define __MAP_HEADER

namespace gltactics {

  constexpr ssize_t DEFAULT_MAPSIZE = 32;

  enum type : uint8_t {
		       AIR = 0,
		       WALL = 1,
		       DOOR = 2
  };

  enum attribute : uint8_t {
			    HORIZONTAL = 1,
			    OPEN = 2
  };

  struct tile {
    type tileType : 4;
    attribute attributeType : 4;
    operator uint8_t() const { return ((uint8_t)tileType << 4) | ((uint8_t)attributeType); }
    tile() {
      tileType = type::AIR;
      attributeType = (attribute)0;
    }
    tile(uint8_t tile) {
      tileType = (type)(tile >> 4);
      attributeType =(attribute)(tile & 15);
    }
  };
  
  template<ssize_t map_size = DEFAULT_MAPSIZE>
  class map {
    tile *layout = new tile[map_size * map_size]; // mapa cuadrado de NxN

  public:
    map(uint8_t fill_map) {
      for(int y = 0; y < map_size; y++) {
	for(int x = 0; x < map_size; x++) {
	  layout[y * map_size + x] = fill_map; // llenar el mapa con este valor uint8_t
	}
      }
    }

    map(std::ifstream input_file) {
      char line[map_size + 3];
      for(int y = 0; y < map_size; y++) {
	input_file.getline(line, map_size + 2); // leemos una linea de hasta 18 caracteres
	for(int x = 0; x < map_size; x++) {
	  layout[y * map_size + x] = line[x] - '0'; // almacenamos el valor como esta en el txt
	}
      }
    }

    // map[{y, x}]
    tile &operator[](std::array<ssize_t, 2> index) {
      return layout[index[0] * map_size + index[1]];
    }

    tile &operator[](ssize_t index) {
      return layout[index];
    }
  };

  // layout[current_position + direction] da referencia al bloque en esa direccion
  enum direction : ssize_t
    {
     up = -DEFAULT_MAPSIZE,
     left = -1,
     right = 1,
     down = DEFAULT_MAPSIZE
    };

  template<size_t map_size = gltactics::DEFAULT_MAPSIZE>
  class map_generator {
    tile m[map_size][map_size] = {0};
  public:
    std::array<size_t, 2> vertical_split(size_t x, size_t y, size_t width, size_t height, size_t hor_door = -1) {
      size_t vert_split;
      do {
	vert_split = (width / 2) + ((rand() % 5) - 2);
      } while (hor_door >= 0 && vert_split == hor_door);
      for(size_t y2 = 0; y2 < height; y2++) {
	m[y + y2][x + vert_split].tileType = type::WALL;
      }
      size_t vert_door = rand() % (height-2) + 1;
      m[y + vert_door][x + vert_split].tileType = type::DOOR;
      return {vert_split, vert_door};
    }
    
    std::array<size_t, 2> horizontal_split(size_t x, size_t y, size_t width, size_t height, size_t vert_door = -1) {
      size_t hor_split;
      do {
	hor_split = (height / 2) + ((rand() % 5) - 2);
      } while (vert_door >= 0 && hor_split == vert_door);
      for(size_t x2 = 0; x2 < width; x2++)
	m[y + hor_split][x + x2].tileType = type::WALL;
      size_t hor_door = rand() % (width-2) + 1;
      m[y + hor_split][x + hor_door].tileType = type::DOOR;
      m[y + hor_split][x + hor_door].attributeType = attribute::HORIZONTAL;
      return {hor_split, hor_door};
    }
    map<map_size> build_map(int seed) {
      srand(seed);
      for(int i = 0; i < map_size; i++) {
	m[0][i].tileType = type::WALL;
	m[i][0].tileType = type::WALL;
	m[map_size-1][i].tileType = type::WALL;
	m[i][map_size-1].tileType = type::WALL;
      }
      auto [v_split, v_door] = vertical_split(0,0,map_size,map_size);
      auto [h1_split, h1_door] =
	horizontal_split(0, 0, v_split, map_size, v_door);
      auto [h2_split, h2_door] =
	horizontal_split(v_split, 0, map_size - v_split, map_size, v_door);
      map<map_size> generatedMap {0};
      for(int y = 0; y < map_size; y++) {
	for(int x = 0; x < map_size; x++) {
	  generatedMap[{y,x}] = m[y][x];
	}
      }
      return generatedMap;
    }
  };
};
#endif
