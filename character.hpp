#ifndef __CHARACTER_HEADER
#define __CHARACTER_HEADER
#include "map.hpp"
namespace gltactics {
  template <ssize_t map_size = DEFAULT_MAPSIZE>
  class character {
    Color _color;
    Vector2 _position;
    map<map_size> &_map;

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

    bool move(direction dir) {
      size_t destination = _position.y * map_size + _position.x + dir;
      if(_map[destination].tileType == gltactics::type::AIR || (_map[destination].attributeType & gltactics::attribute::OPEN)) {
	_position.y = destination / map_size;
	_position.x = destination % map_size;
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
  };
};
#endif
