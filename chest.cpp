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
  std::uniform_int_distribution<> dis(0,3);
  return choices[dis(random_engine)];
};


gltactics::chest::chest(std::mt19937_64 &random_engine,
			gltactics::item a, gltactics::item b, gltactics::item c)
  : random_engine{random_engine}, choices{a,b,c} {  
}
