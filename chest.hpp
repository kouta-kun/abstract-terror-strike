#ifndef __CHEST_HPP
#define __CHEST_HPP
#include <optional>
#include <random>
#include <array>
#include "constants.hpp"

namespace gltactics {
  template <ssize_t map_size>
  class character;
};

namespace gltactics {
  typedef void (*ItemUseFn)(gltactics::character<DEFAULT_MAPSIZE>*);
  class item {
    std::optional<ItemUseFn> use_action;
  public:
    int id;
    bool has_use_action();
    ItemUseFn get_use_fn();
    item(int id);
    item(int id, ItemUseFn use_action);
  };

  class chest {
    std::array<gltactics::item, 3> choices;
    std::mt19937_64 &random_engine;

  public:
    gltactics::item pick_item();
    chest(std::mt19937_64 &random_engine,
	  gltactics::item a, gltactics::item b, gltactics::item c);
  };
};

#endif
