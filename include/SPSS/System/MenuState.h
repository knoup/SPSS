#ifndef SPSS_MENUSTATE_H_INCLUDED
#define SPSS_MENUSTATE_H_INCLUDED

////////////////////////////////////////////////////////////////////
/// spss::MenuState inherits from spss::State and has the ability
/// to add menu items which the user can click. These menu items
/// can also be assigned functions that are to be called upon click.
///
/// MenuStates must be given a type parameter F, and a pointer to an
/// object of that type in its constructor. This pointer is stored
/// as a data member, m_functionScope.
///
/// When calling addMenuItem(), any public member functions belonging
/// to F and with a void return type can be added as the second
/// parameter (via std::bind). When this menu item is activated,
/// either via clicking it or pressing  a specified keyboard shortcut,
/// m_functionScope is used to call the function.
////////////////////////////////////////////////////////////////////

#include <SPSS/System/State.h>
#include <SPSS/Graphics/Scrollbar.h>
#include <SPSS/Util/Function.h>
#include <SPSS/Util/Math.h>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <any>
#include <string>
#include <random>
#include <chrono>

namespace spss {

	class MenuState : public State {
	  private:
		////////////////////////////////////////////////////////////
		///
		/// We'll represent the menu items with its own struct
		///
		/// The first element, a boolean, represents whether the
		/// element is currently being moused over or not
		///
		/// The second element, a function pointer, will be used to
		/// determine what happens when the item is clicked on
		///
		/// The third element, an sf::Text object, is what the user
		/// sees on the menu screen
		///
		/// The fourth element will be an integer representing a key
		/// (sf::Keyboard). This is optional and can be used as a
		/// shortcut to simulate a click on this MenuItem
		///
		/// The fifth element is a bool specifying whether this
		/// MenuItem has been specifically positioned by the user.
		/// If so, it won't be included in the scroll view.
		///
		/// The sixth and final element is only used if the fifth
		/// is true. ratioPos's valid value ranges are from
		/// 0-1, as it is defined not in pixels, but rather as a ratio
		/// of the window size, much like sf::ViewPort. For example,
		/// {0.5, 0} would  horizontally center the text at the highest
		/// vertical point.
		///
		/// Positions are specified this way so that they can be auto-
		/// matically repositioned as needed upon window resizing.
		///
		////////////////////////////////////////////////////////////
		struct MenuItem {
			MenuItem(bool                _mousedOver,
			         Function<std::any>  _boundFunction,
			         sf::Text&           _text,
			         int                 _keyCode,
			         bool                _manualPos,
			         const sf::Vector2f& _ratioPos = {})
			            : mousedOver{_mousedOver},
			              boundFunction{_boundFunction},
			              text{_text},
			              keyCode{_keyCode},
			              manualPos{_manualPos},
			              ratioPos{_ratioPos} {};

			bool               mousedOver;
			Function<std::any> boundFunction;
			sf::Text           text;
			int                keyCode;
			bool               manualPos;
			sf::Vector2f       ratioPos;
		};

	  public:
		////////////////////////////////////////////////////////////
		/// \brief Construct the menu state
		///
		/// In order to be able to call a class's functions, a
		/// pointer to an instance of the class needs to be
		/// supplied. This is the second argument.
		///
		///
		/// \param _w            RenderWindow used to draw the state
		/// \param _o            The callable function scope
		/// \param _f            Font used to draw text
		/// \param _titleStr     The default title string
		/// \param _titleAutoPos If false, title will always be at the very top
		///
		////////////////////////////////////////////////////////////
		MenuState(sf::RenderWindow&  _w,
		          const sf::Font&    _f,
		          const std::string& _titleStr = "",
		          bool               _titleAutoPos = true);

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
		virtual void update(int _timeslice);

		////////////////////////////////////////////////////////////
		/// \brief Draw
		///
		////////////////////////////////////////////////////////////
		virtual void draw();

		////////////////////////////////////////////////////////////
		/// \brief Add a clickable menu item
		///
		/// Creates a menu item that, upon being clicked, can call
		/// any public member function of the function scope object
		/// (the template used to create this class). A null function
		/// can be specified, in which case nothing will happen upon
		/// click. Additionally, a sf::Keyboard keycode can also be
		/// provided, which will emulate a mouse click on this item
		/// upon being pressed.
		///
		///
		/// \param _string  The menu item's description
		/// \param f        The callable function scope
		/// \param _keyCode The keyboard shortcut to emulate a mouse click
		///
		////////////////////////////////////////////////////////////

		virtual void addMenuItem(const std::string&       _string,
		                         spss::Function<std::any> f        = nullptr,
		                         int                      _keyCode = sf::Keyboard::Unknown);

		////////////////////////////////////////////////////////////
		/// \brief Add a clickable menu item
		///
		/// This function does the exact same thing as the function
		/// above, but also takes in a relative position. This pos-
		/// ition's x and y values can range from 0-1, and it is de-
		/// fined not in pixels, but rather as a ratio of the window
		/// size, much like sf::ViewPort.
		///
		/// Positions are specified this way so that they can be auto-
		/// matically repositioned as needed upon window resizing.
		///
		/// Note that MenuItems added with a position specified will not
		/// be a part of the centered view to which the scrollbar applies.
		///
		///
		/// \param _string  The menu item's description
		/// \param _pos     The menu item's position as a ratio
		/// \param f        The callable function scope
		/// \param _keyCode The keyboard shortcut to emulate a mouse click
		///
		////////////////////////////////////////////////////////////
		virtual void addMenuItem(const std::string&       _string,
								 const sf::Vector2f&      _pos,
		                         spss::Function<std::any> f        = nullptr,
		                         int                      _keyCode = sf::Keyboard::Unknown);

		////////////////////////////////////////////////////////////
		/// \brief Adds an empty line
		///
		/// Simply a short way of calling addMenuItem("").
		///
		/// \see addMenuItem
		///
		////////////////////////////////////////////////////////////
		void addGap();

		////////////////////////////////////////////////////////////
		/// \brief Sets the scrollbar's color
		///
		////////////////////////////////////////////////////////////
		void setScrollbarColor(sf::Color _c);

	  private:
		////////////////////////////////////////////////////////////
		/// \brief Resets both m_view and m_backgroundView
		///
		/// Resets both views to encompass the entirety of the new
		/// window size.
		///
		/// \param _newSize The new size of the window
		///
		////////////////////////////////////////////////////////////
		void resetViews(sf::Vector2u _newSize);

		////////////////////////////////////////////////////////////
		/// \brief Adjusts m_view's viewport
		///
		/// Appropriately sets up m_view's viewport to start just
		/// under the title text.
		///
		////////////////////////////////////////////////////////////
		void adjustViewport();

		////////////////////////////////////////////////////////////
		/// \brief Resets the title text's position
		///
		/// Positions the title text at the very top of the window.
		///
		////////////////////////////////////////////////////////////
		void resetTitlePosition();

		////////////////////////////////////////////////////////////
		/// \brief Adjusts the position of the menu items
		///
		/// Positions the menu items in order with the appropriate
		/// vertical spacing between them.
		///
		////////////////////////////////////////////////////////////
		void adjustMenuItems();

		////////////////////////////////////////////////////////////
		/// \brief Centers the title text and menu items
		///
		/// Appropriately positions the title text and adjusts
		/// m_view's viewport so that the distance between the top
		/// of the window and the top of the title text is roughly
		/// equivalent to the distance between the bottom of the
		/// window and the bottom of the last menu item.
		///
		/// Note that if m_titleAutoPos is set to false, this function
		/// won't be called.
		///
		////////////////////////////////////////////////////////////
		void adjustBoundaries();

		////////////////////////////////////////////////////////////
		/// \brief Detects mouseclicks
		///
		/// Detects mouseclicks and mouseovers
		///
		////////////////////////////////////////////////////////////
		void detectMouseClicks();

		////////////////////////////////////////////////////////////
		/// \brief Update the title text
		///
		/// Randomises title color, scale, and rotation if these
		/// options have been enabled.
		///
		/// \param _timeslice The timeslice value
		///
		///
		/// \see setRandomisedColors
		/// \see setScalingText
		/// \see setRotatingText
		///
		////////////////////////////////////////////////////////////
		void updateTitleText(int _timeslice);

		////////////////////////////////////////////////////////////
		/// \brief Checks if a menu item is being moused over
		///
		/// \param _menuItem The menu item to check
		///
		////////////////////////////////////////////////////////////
		bool isMousedOver(const MenuItem& _menuItem) const;

		////////////////////////////////////////////////////////////
		/// \brief Checks if a menu item's bound function is null
		///
		/// \param _menuItem The menu item to check
		///
		////////////////////////////////////////////////////////////
		bool isFunctionNull(const MenuItem& _menuItem) const;

		////////////////////////////////////////////////////////////
		/// \brief Checks if an event's type is a LMB press
		///
		/// \param _event The captured event
		///
		////////////////////////////////////////////////////////////
		bool lmbPressed(sf::Event& _event);

		////////////////////////////////////////////////////////////
		/// \brief Checks if an event's type is a LMB release
		///
		/// \param _event The captured event
		///
		////////////////////////////////////////////////////////////
		bool lmbReleased(sf::Event& _event);

	  protected:
		////////////////////////////////////////////////////////////
		/// \brief Called when the window is resized
		///
		/// \param _newSize The new window size
		///
		////////////////////////////////////////////////////////////
		virtual void onResize(sf::Vector2u _newSize);

		////////////////////////////////////////////////////////////
		/// \brief Enable or disable the title text's random colors
		///
		/// \param _b        The boolean value
		/// \param _duration MS the title's color persists for before being randomised again
		///
		////////////////////////////////////////////////////////////
		void setRandomisedColors(bool _b, int _duration = 500);

		////////////////////////////////////////////////////////////
		/// \brief Enable or disable the title text's scaling
		///
		/// \param _b        The boolean value
		/// \param _minScale The minimum possible scaling value
		/// \param _masScale The maximum possible scaling value
		///
		////////////////////////////////////////////////////////////
		void setScalingText(bool _b, float _minScale = 0.75, float _maxScale = 1.25);

		////////////////////////////////////////////////////////////
		/// \brief Enable or disable the title text's rotation
		///
		/// \param _b        The boolean value
		/// \param _minScale The maximum degree of rotation
		///
		////////////////////////////////////////////////////////////
		void setRotatingText(bool _b, float _rotationDegrees = 10);

		////////////////////////////////////////////////////////////
		/// \brief Set the title text string
		///
		/// \param _str The new title text's string
		///
		////////////////////////////////////////////////////////////
		void setTitleString(const std::string& _str);

		////////////////////////////////////////////////////////////
		/// \brief Set the title text size
		///
		/// \param _size The new title text's size
		///
		////////////////////////////////////////////////////////////
		void setTitleSize(unsigned int _size);

		////////////////////////////////////////////////////////////
		/// \brief Set the title text color
		///
		/// \param _color The new title text's color
		///
		////////////////////////////////////////////////////////////
		void setTitleColor(sf::Color _color);

		////////////////////////////////////////////////////////////
		/// \brief Set the menu items' size
		///
		/// \param _size The new menu items' size
		///
		////////////////////////////////////////////////////////////
		void setMenuItemsSize(unsigned int _size);

		////////////////////////////////////////////////////////////
		/// \brief Set the menu items' color
		///
		/// \param _color The new menu items' color
		///
		////////////////////////////////////////////////////////////
		void setMenuItemsColor(sf::Color _color);

		////////////////////////////////////////////////////////////
		/// \brief Set the menu items' higlighted color
		///
		/// \param _color The new menu items' higlighted color
		///
		////////////////////////////////////////////////////////////
		void setHighlightColor(sf::Color _color);

		////////////////////////////////////////////////////////////
		/// \brief Set the menu items' highlighted scale
		///
		/// \param _f The new menu items' highlighted scale
		///
		////////////////////////////////////////////////////////////
		void setHighlightScale(float _f);

		////////////////////////////////////////////////////////////
		/// \brief Get the space above the the beginning of m_menuItems
		///
		////////////////////////////////////////////////////////////
		float getMenuOffset() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the distance between the first/last menu items
		///
		////////////////////////////////////////////////////////////
		float getMenuHeight() const;

		////////////////////////////////////////////////////////////
		/// \brief (Re)initialize scrollbar parameters as appropriate
		///
		////////////////////////////////////////////////////////////
		void setupScrollbar();

		///////////////////////////////////////////////////////////
		//Data members --------------------------------------------
		///////////////////////////////////////////////////////////

	  private:
		const sf::Font&       m_font;                  ///< Font used to display the title and menu items
		std::vector<MenuItem> m_menuItems;             ///< The menu items
		sf::View              m_view;                  ///< The view used to draw the menu items
		sf::View              m_backgroundView;        ///< The view used to draw the title text
		sf::Text              m_titleText;             ///< The title text
		bool                  m_titleAutoPos;          ///< If false, the title will always be at the very top to save space
		spss::Scrollbar       m_scrollbar;             ///< The scrollbar
		bool                  m_randomiseTextColor;    ///< Is the title's color randomised?
		int                   m_randomColorDurationMS; ///< How many MS the title's color persists for before it is randomised again
		bool                  m_scaleText;             ///< Is the title being scaled up and down?
		float                 m_minScale;              ///< The title text's minimum possible scaling value
		float                 m_maxScale;              ///< The title text's maximum possible scaling value
		bool                  m_rotateText;            ///< Is the title text being rotated?
		float                 m_rotationDegrees;       ///< The title text's maximum rotation in degrees
		sf::Color             m_highlightColor;        ///< The color of highlighted menu items
		float                 m_highlightScale;        ///< The scaling value for highlighted menu items
		                                               //---------------------------------------------
	};

} //namespace spss

#endif // SPSS_MENUSTATE_H_INCLUDED
