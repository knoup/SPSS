#ifndef SCROLLBAR_H_INCLUDED
#define SCROLLBAR_H_INCLUDED

////////////////////////////////////////////////////////////////////
/// spss::Scrollbar represents and functions as a conventional scro-
/// llbar. It will automatically size its inner section as needed,
/// according to the ratio of visible : total content, and will re-
/// center the associated view according to its position.
////////////////////////////////////////////////////////////////////

#include <SPSS/Graphics/DrawableToWindow.h>
#include <SFML/Graphics.hpp>

namespace spss {

	class Scrollbar : public spss::DrawableToWindow {
	  public:
	  	////////////////////////////////////////////////////////////
	  	///
	  	/// The three types of Anchor behave as follows:
	  	///
	  	/// NONE: no special behaviour.
	  	///
	  	/// SOFT: if the scrollbar is already at the bottommost
	  	/// position when reset() is called, it will ensure it
	  	/// stays there.
	  	///
	  	/// HARD: the scrollbar is always set to the bottommost
	  	/// position when reset() is called.
	  	///
	  	////////////////////////////////////////////////////////////
	  	enum class Anchor
	  	{
			NONE,
			SOFT,
			HARD
	  	};

		////////////////////////////////////////////////////////////
		/// \brief Construct the Scrollbar
		///
		/// \param _window     The window to be drawn to
		/// \param _bgView     The background view
		/// \param _scrollView The view the scrollbar will be applied to
		/// \param _size       The size (in pixels)
		/// \param _pos        The position (in pixels)
		/// \param _min        The min Y value for the scrollview's center
		/// \param _max        The max Y value for the scrollview's center
		///
		////////////////////////////////////////////////////////////
		Scrollbar(sf::RenderWindow*   _window,
		          const sf::View&     _bgView,
		          sf::View&           _scrollView,
		          const sf::Vector2f& _size = {},
		          const sf::Vector2f& _pos  = {},
		          float               _min  = 0,
		          float               _max  = 0);

		////////////////////////////////////////////////////////////
		/// \brief Reset the scrollbar's init parameters
		///
		/// \param _size       The size (in pixels)
		/// \param _pos        The position (in pixels)
		/// \param _min        The min Y value for the scrollview's center
		/// \param _max        The max Y value for the scrollview's center
		///
		////////////////////////////////////////////////////////////
		void reset(const sf::Vector2f& _size,
		           const sf::Vector2f& _pos,
		           float               _min,
		           float               _max);

		////////////////////////////////////////////////////////////
		/// \brief  Sets whether the mouse wheel can be used as input
		///
		/// \param _b The boolean value
		///
		////////////////////////////////////////////////////////////
		void setScrollWithWheel(bool _b);

		////////////////////////////////////////////////////////////
		/// \brief  Sets the scrollbar's active status
		///
		/// \param _b The boolean value
		///
		////////////////////////////////////////////////////////////
		void setActive(bool _b);

		////////////////////////////////////////////////////////////
		/// \brief Get input
		///
		/// The optional _mouseBounds parameter specifies the bounds
		/// in which mouse wheel scrolling works. Leave blank to be
		/// able to scroll with the mouse in any position.
		///
		/// \param _event       A reference to a captured event
		/// \param _mouseBounds Scroll only if MMB is moved within these bounds
		///
		////////////////////////////////////////////////////////////
		void getInput(sf::Event& _event, const sf::FloatRect& _mouseBounds = {});

		////////////////////////////////////////////////////////////
		/// \brief Update
		///
		////////////////////////////////////////////////////////////
		void update();

		////////////////////////////////////////////////////////////
		/// \brief Draw the text to a render window
		///
		/// \param target Render window to draw to
		/// \param states Current render states
		///
		////////////////////////////////////////////////////////////
		void draw(sf::RenderWindow& window, sf::RenderStates states) const;

		////////////////////////////////////////////////////////////
		/// \brief Set the scrollbar's color
		///
		/// \param _c The color
		///
		////////////////////////////////////////////////////////////
		void setColor(const sf::Color& _c);

		////////////////////////////////////////////////////////////
		/// \brief Get the scrollbar's color
		///
		////////////////////////////////////////////////////////////
		const sf::Color& getColor() const;

		////////////////////////////////////////////////////////////
		/// \brief Sets the scrollbar's anchor type
		///
		/// \param _a The anchor type
		///
		////////////////////////////////////////////////////////////
		void setAnchor(Anchor _a);

		////////////////////////////////////////////////////////////
		/// \brief Get the scrollbar's anchor type
		///
		////////////////////////////////////////////////////////////
		Anchor getAnchor() const;

		////////////////////////////////////////////////////////////
		/// \brief Snap into the uppermost possible position
		///
		////////////////////////////////////////////////////////////
		void snapToTop();

		////////////////////////////////////////////////////////////
		/// \brief Snap into the bottommost possible position
		///
		////////////////////////////////////////////////////////////
		void snapToBottom();

	  private:
	  	////////////////////////////////////////////////////////////
		/// \brief Is the scrollbar at the uppermost possible position?
		///
		////////////////////////////////////////////////////////////
	  	bool atTop() const;

	  	////////////////////////////////////////////////////////////
		/// \brief Is the scrollbar at the botommost possible position?
		///
		////////////////////////////////////////////////////////////
	  	bool atBottom() const;

		////////////////////////////////////////////////////////////
		/// \brief Is the scrollbar being moused over?
		///
		////////////////////////////////////////////////////////////
		bool mousedOver() const;

		////////////////////////////////////////////////////////////
		/// \brief Scroll up or down a notch
		///
		/// \param _up Up or down?
		///
		////////////////////////////////////////////////////////////
		void scroll(bool _up);

		////////////////////////////////////////////////////////////
		/// \brief Move the scrollbar if it's being dragged
		///
		////////////////////////////////////////////////////////////
		void drag();

		////////////////////////////////////////////////////////////
		/// \brief Calculate the view's center based on the scrollbar
		///
		////////////////////////////////////////////////////////////
		void calculateNewViewCenter();

		///////////////////////////////////////////////////////////
		//Data members --------------------------------------------
		///////////////////////////////////////////////////////////
		mutable sf::RenderWindow* m_window;          ///< The window to draw the scrollbar in
		const sf::View&           m_backgroundView;  ///< The background view to which the scrollbar will be drawn
		sf::View&                 m_scrollView;      ///< The view to which the scrollbar applies
		sf::Color                 m_color;           ///< The color of the scrollbar
		bool                      m_scrollWithWheel; ///< Can it be scrolled with the mouse wheel?
		bool                      m_active;          ///< Is the scrollbar active?
		mutable bool              m_dragging;        ///< Is the scrollbar being dragged?
		sf::RectangleShape        m_outer;           ///< The outer part of the scrollbar
		sf::RectangleShape        m_inner;           ///< The inner part of the scrollbar
		float                     m_minCenterY;      ///< The minimum value for the scrollable view's center Y
		float                     m_maxCenterY;      ///< The maximum value for the scrollable view's center Y
		Anchor                    m_anchor;          ///< The anchor type (see enum declaration for details)
	};

} // namespace spss

#endif // SCROLLBAR_H_INCLUDED
