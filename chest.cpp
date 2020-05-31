#include "chest.hpp"

gltactics::item::item(int id) : id{id} {};

gltactics::item::item(int id, gltactics::ItemUseFn use_action)
  : id{id}, use_action{use_action} {}

bool gltactics::item::has_use_action() {
  return (bool)(this->use_action);
};

gltactics::ItemUseFn gltactics::item::get_use_fn() {
  return *use_action;
}


gltactics::item gltactics::chest::pick_item() {
    return item;
};


gltactics::chest::chest(gltactics::item a)
  : item{a} {
}

std::strong_ordering gltactics::operator<=>(gltactics::item a, gltactics::item b) {
    return a.id <=> b.id;
}
