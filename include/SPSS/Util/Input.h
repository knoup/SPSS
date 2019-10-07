#ifndef UTILINPUT_H_INCLUDED
#define UTILINPUT_H_INCLUDED

#include <SFML/Window/Event.hpp>

namespace spss::Util::Input {

	////////////////////////////////////////////////////////////
	/// \brief Was the left mouse button pressed?
	///
	/// \param _event A reference to a captured event
	///
	////////////////////////////////////////////////////////////
	inline bool lmbPressed(sf::Event& _event) {
		if (_event.type == sf::Event::MouseButtonPressed &&
		    _event.mouseButton.button == sf::Mouse::Left) {
			return true;
		}
		else {
			return false;
		}
	}

	////////////////////////////////////////////////////////////
	/// \brief Was the left mouse button released?
	///
	/// \param _event A reference to a captured event
	///
	////////////////////////////////////////////////////////////
	inline bool lmbReleased(sf::Event& _event) {
		if (_event.type == sf::Event::MouseButtonReleased &&
		    _event.mouseButton.button == sf::Mouse::Left) {
			return true;
		}
		else {
			return false;
		}
	}

} // namespace spss::Util::Input

#endif // UTILINPUT_H_INCLUDED
