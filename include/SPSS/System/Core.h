#pragma once

////////////////////////////////////////////////////////////////////
/// spss::Core manages the fundementals of a typical program loop.
///
/// It runs the typical get input -> update -> draw loop with a
/// fixed timestep.
///
/// It also holds a stack of States, and calls the aforementioned
/// functions (getInput(), update(), draw()) of the state at the
/// top of the stack, and possibly the update() and draw() functions
/// of the penultimate state.
////////////////////////////////////////////////////////////////////

#include <SPSS/System/State.h>
#include <SPSS/Util/Singleton.h>
#include <SFML/Graphics.hpp>

#include <memory>
#include <vector>

namespace spss {

	class Core : public spss::Singleton<Core> {
	  public:
		////////////////////////////////////////////////////////////
		/// \brief Default constructor
		///
		/// \param Reference to the primary sf::RenderWindow object
		/// \param The timeslice value used by update()
		///
		////////////////////////////////////////////////////////////
		Core(sf::RenderWindow& _window,
		     const int         _timeslice);

		////////////////////////////////////////////////////////////
		/// \brief Begins the program loop
		///
		/// With a fixed timestep this function calls the
		/// getInput(), update(), and draw() functions of
		/// the state currently at the top of the state stack
		///
		////////////////////////////////////////////////////////////
		virtual void run();

		////////////////////////////////////////////////////////////
		/// \brief Gets input
		///
		/// In addition to calling getInput() for the current state
		/// at the top of the stack, this function also checks if
		/// the window has been closed and terminates the main loop
		/// if so.
		///
		////////////////////////////////////////////////////////////
		void getInput();

		////////////////////////////////////////////////////////////
		/// \brief Update
		///
		/// Calls the update function for the state at the top of the
		/// state stack. Also calls the penultimate state's update()
		/// function if the state at the top of the stack is visible
		/// over other states.
		///
		/// \param Timeslice value
		///
		////////////////////////////////////////////////////////////
		void update(int _timeslice);

		////////////////////////////////////////////////////////////
		/// \brief Draw
		///
		/// Calls the draw function for the state at the top of the
		/// state stack. Also calls the penultimate state's draw()
		/// function if the state at the top of the stack is visible
		/// over other states.
		///
		////////////////////////////////////////////////////////////
		void draw();

		////////////////////////////////////////////////////////////
		/// \brief Push state
		///
		/// Pushes a state to the stack.
		///
		/// \param State uptr (will be moved)
		///
		////////////////////////////////////////////////////////////
		void pushState(std::unique_ptr<spss::State> _state);

		////////////////////////////////////////////////////////////
		/// \brief Pop state
		///
		/// Pops the state at the top of the state stack.
		///
		////////////////////////////////////////////////////////////
		void popState();

		////////////////////////////////////////////////////////////
		/// \brief Exits the loop
		///
		/// Closes m_window, which in turn ends the loop in run()
		///
		////////////////////////////////////////////////////////////
		void exit();

		////////////////////////////////////////////////////////////
		/// \brief Get the current state at the top of the stack
		///
		////////////////////////////////////////////////////////////
		const State* getTopState() const;

		///////////////////////////////////////////////////////////
		//Data members --------------------------------------------
		///////////////////////////////////////////////////////////
	  public:
		sf::RenderWindow& m_window; ///<The primary application window
	  private:
		std::vector<std::unique_ptr<spss::State>> m_stateStack; ///<The state stack
		const int                                 m_timeslice;  ///<The timeslice value used in update()
	};

} //namespace spss
