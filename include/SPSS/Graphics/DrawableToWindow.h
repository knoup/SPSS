#pragma once

////////////////////////////////////////////////////////////////////
/// spss::DrawableToWindow inherits from sf::Drawable and is meant
/// to be used for classes that can only be drawn to an sf::RenderWindow
/// (such as classes that need to know the window size, for example).
///
/// All it does is overload the draw function that takes in an
/// sf::RenderTarget& and checks if the render target is a window.
/// If it's not, nothing happens; if it is, a user-defined
/// draw(RenderWindow, States) function is called.
////////////////////////////////////////////////////////////////////

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace spss {
	class DrawableToWindow : public sf::Drawable {
	  public:
	  	////////////////////////////////////////////////////////////
		/// \brief Draw the text to a render window
		///
		/// This function is abstract and must be defined by any
		/// inheritor.
		///
		/// \param target Render window to draw to
		/// \param states Current render states
		///
		////////////////////////////////////////////////////////////

		virtual void draw(sf::RenderWindow& window, sf::RenderStates states) const = 0;

		////////////////////////////////////////////////////////////
		/// \brief Draw the text to a render target
		///
		/// Since we only want this to be drawable to sf::Render-
		/// Windows, and since sf::Drawable requires this function to
		/// be overloaded, we'll dynamically cast target to a sf::Render-
		/// Window. If that's not possible, then draw() will do nothing.
		///
		/// \param target Render target to draw to
		/// \param states Current render states
		///
		////////////////////////////////////////////////////////////

		inline void draw(sf::RenderTarget& target, sf::RenderStates states) const {
			sf::RenderWindow* w{dynamic_cast<sf::RenderWindow*>(&target)};
			if (w == nullptr) {
				return;
			}
			draw(*w, states);
		}
	};
} // namespace spss
