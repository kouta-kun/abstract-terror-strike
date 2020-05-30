#ifndef ABSTRACT_TERROR_STRIKE_CHARACTER_HPP
#define ABSTRACT_TERROR_STRIKE_CHARACTER_HPP
#include "map.hpp"
#include "chest.hpp"
#include "raylib.h"

namespace gltactics {
  template <ssize_t map_size = DEFAULT_MAPSIZE>
  class character {
    map<map_size> &_map;
    Color _color;
    Vector2 _position;
    std::vector<gltactics::item> _inventory;

    void openDoor(ssize_t x, ssize_t y, gltactics::map<> &mapRef) {
      gltactics::type tileType = mapRef[{y, x}].tileType;
      if (tileType == gltactics::type::DOOR) {
	bool isLocked = mapRef[{y, x}].attributeType & gltactics::attribute::LOCKED;
	if (!isLocked) {
	  mapRef[{y, x}].attributeType = (gltactics::attribute) (mapRef[{y, x}].attributeType ^
								 gltactics::attribute::OPEN);
	}
      }
    }
    
  public:
    character(Color color, Vector2 position, map<map_size> &_map) : _color{color}, _position{position}, _map{_map} {}
    
    map<map_size> &map() {
      return _map;
    }
    
    const Color &color() const {
      return _color;
    }

    void setColor(Color color) {
      this->_color = color;
    }

    std::vector<int> getInventoryList() {
      std::vector<int> ids;
      for(auto &item : this->_inventory) {
	ids.push_back(item.id);
      }
      return ids;
    }
    
    void useEnvironment() {
      gltactics::overMapRange(*this,
		   [&](ssize_t x, ssize_t y, gltactics::map<> &mapRef, bool &_) {
		     openDoor(x,y,mapRef);
		   });
    }

    bool move(direction dir) {
      size_t destination = (size_t)(_position.y * map_size + _position.x) + dir;
      gltactics::type blockType = _map[destination].tileType;
      if(blockType == gltactics::type::AIR || (blockType == gltactics::type::DOOR && (_map[destination].attributeType & gltactics::attribute::OPEN) > 0)) {
	_position.y = float(destination / map_size);
	_position.x = float(destination % map_size);
	return true;
      }
      return false;
    }

    const Vector2 &position() const {
      return _position;
    }

    Vector3 position3d() const {
      return Vector3 { _position.x, 0.0f, _position.y };
    }

    void setPosition(Vector2 position) {
      this->_position = position;
    }

    template<gltactics::attribute onAttribute>
    static void openDoors(gltactics::character<> *character) {
      gltactics::overMapRange(*character,
			      [](ssize_t x, ssize_t y, gltactics::map<> &mapRef, bool&_) {
				gltactics::tile tile = mapRef[{y,x}];
				if(tile.tileType == gltactics::type::DOOR) {
				  if(tile.attributeType & onAttribute) {
				    tile.attributeType =
				      (gltactics::attribute)(tile.attributeType & (~gltactics::attribute::LOCKED));
				  }
				}
			      });
    }
  };
};
#endif
