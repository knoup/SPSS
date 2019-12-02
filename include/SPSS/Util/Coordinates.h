#ifndef COORDINATES_H_INCLUDED
#define COORDINATES_H_INCLUDED

#include <SFML/System/Vector2.hpp>

////////////////////////////////////////////////////////////////////
/// Contains some functions useful when you're dealing with a 2D
/// coordinate-based system.
////////////////////////////////////////////////////////////////////

namespace spss::Util::Coordinates {

	inline int getIndex(sf::Vector2i _coords, int _rowLength) {
		return {(_coords.y * _rowLength) + _coords.x};
	}

	inline sf::Vector2i getCoords(int _index, int _rowLength) {
		return {_index % _rowLength, _index / _rowLength};
	}

	inline sf::Vector2i northOf(sf::Vector2i _coords) {
		return {_coords.x, _coords.y - 1};
	}
	inline sf::Vector2i southOf(sf::Vector2i _coords) {
		return {_coords.x, _coords.y + 1};
	}
	inline sf::Vector2i eastOf(sf::Vector2i _coords) {
		return {_coords.x + 1, _coords.y};
	}
	inline sf::Vector2i westOf(sf::Vector2i _coords) {
		return {_coords.x - 1, _coords.y};
	}

	inline bool outOfRange(sf::Vector2i _coords, const sf::Vector2i _size) {
		if (_coords.x < 0 || _coords.y < 0 || _coords.x >= _size.x ||
		    _coords.y >= _size.y) {
			return true;
		}
		return false;
	}

	inline bool outOfRange(int _index, const sf::Vector2i _size) {
		int minIndex{0};
		int maxIndex{(_size.x * _size.y) - 1};

		if (_index < minIndex || _index > maxIndex) {
			return true;
		}

		return false;
	}

} // namespace spss::Util::Coordinates

#endif // COORDINATES_H_INCLUDED
