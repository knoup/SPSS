#pragma once

////////////////////////////////////////////////////////////////////
/// spss::State represents the behaviour of a certain state.
///
/// It has the core functions (getInput(), update(), draw()),
/// in addition to onResize() and onStateSwitch().
///
/// It is an abstract class; it's up to the user to inherit from State
/// and overload at least the destructor, getInput(), update(), and draw()
/// to suit their needs.
////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>

namespace spss {

	class State {
	  public:
		State(sf::RenderWindow& _w);
		virtual ~State() = 0;

		////////////////////////////////////////////////////////////
		/// \brief Get input
		///
		/// \param A reference to a captured event
		///
		////////////////////////////////////////////////////////////
		virtual void getInput(sf::Event& _event);

		////////////////////////////////////////////////////////////
		/// \brief Update
		///
		/// \param Timeslice value
		///
		////////////////////////////////////////////////////////////
		virtual void update(int _timeslice) = 0;

		////////////////////////////////////////////////////////////
		/// \brief Draw
		///
		////////////////////////////////////////////////////////////
		virtual void draw() const = 0;

		////////////////////////////////////////////////////////////
		/// \brief On state switch
		///
		/// Called by the new current state when a state is popped
		/// from the states stack.
		/// By default, all it does is call the onResize() function,
		/// ensuring the views aren't all messed up.
		///
		////////////////////////////////////////////////////////////
		virtual void onStateSwitch();

		void pollPreviousState(bool _b);
		void drawPreviousState(bool _b);
		void updatePreviousState(bool _b);

		bool previousStatePolled() const;
		bool previousStateDrawn() const;
		bool previousStateUpdated() const;

	  protected:
		////////////////////////////////////////////////////////////
		/// \brief On resize
		///
		/// \param The new window size
		///
		/// Called when the window is resized
		///
		////////////////////////////////////////////////////////////
		virtual void onResize(sf::Vector2u _newSize);

		///////////////////////////////////////////////////////////
		//Data members --------------------------------------------
		///////////////////////////////////////////////////////////
	  protected:
		sf::RenderWindow& m_window;

	  private:
	  	bool m_pollPreviousState;
		bool m_drawPreviousState;
		bool m_updatePreviousState;
	};

} //namespace spss
