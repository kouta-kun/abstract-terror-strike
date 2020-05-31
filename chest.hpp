#ifndef __CHEST_HPP
#define __CHEST_HPP
#include <optional>
#include <random>
#include <compare>
#include <array>
#include "constants.hpp"

namespace gltactics {
  template <size_t map_size>
  class character;
};

namespace gltactics {
  typedef bool (*ItemUseFn)(gltactics::character<DEFAULT_MAPSIZE>*);
  class item {
    std::optional<ItemUseFn> use_action;
  public:
    int id;
    bool has_use_action();
    ItemUseFn get_use_fn();
    item(int id);
    item(int id, ItemUseFn use_action);
  };
  std::strong_ordering operator<=>(item a, item b);


    class chest {
    gltactics::item item;

  public:
    gltactics::item pick_item();
    chest(gltactics::item a);
  };
};

#endif
