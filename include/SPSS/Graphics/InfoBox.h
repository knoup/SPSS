#ifndef INFOBOX_H_INCLUDED
#define INFOBOX_H_INCLUDED

////////////////////////////////////////////////////////////////////
/// spss::InfoBox is a resizable and movable box to which InfoBoxMessages
/// can be added. If the text is too tall, a scrollbar will be automatic-
/// ally activated. Useful for tooltip-style/informational boxes.
////////////////////////////////////////////////////////////////////

#include <SPSS/Graphics/MulticolorText.h>
#include <SPSS/Graphics/InfoBoxMessage.h>
#include <SPSS/Graphics/Scrollbar.h>

#include <SFML/Graphics.hpp>

namespace spss {

	class InfoBox : public sf::Drawable {
	  public:
		////////////////////////////////////////////////////////////
		/// \brief Construct the InfoBox
		///
		/// \param _size     The size (in pixels)
		/// \param _position The position (in pixels)
		/// \param _font     Font used to draw text
		/// \param _charSize The character size
		///
		////////////////////////////////////////////////////////////
		InfoBox(const sf::Vector2f& _size,
		        const sf::Vector2f& _position,
		        const sf::Font&     _font,
		        unsigned            _charSize = 20);

		////////////////////////////////////////////////////////////
		/// \brief Add a message (title + content) entry
		///
		/// \param _msg The message to add
		///
		////////////////////////////////////////////////////////////
		void appendMessage(const Message& _msg);

		////////////////////////////////////////////////////////////
		/// \brief Get input
		///
		/// \param A reference to a captured event
		///
		////////////////////////////////////////////////////////////
		void getInput(sf::Event& _event);

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
		/// \brief Draw the text to a render target
		///
		/// Since we only want InfoBox to be drawable to sf::Render-
		/// Windows, and since sf::Drawable requires this function to
		/// be overloaded, we'll dynamically cast target to a sf::Render-
		/// Window. If that's not possible, then draw() will do nothing.
		///
		/// \param target Render target to draw to
		/// \param states Current render states
		///
		////////////////////////////////////////////////////////////
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		////////////////////////////////////////////////////////////
		/// \brief Set the box's fill color
		///
		/// \param _color The color
		///
		////////////////////////////////////////////////////////////
		void setColor(const sf::Color& _color);

		////////////////////////////////////////////////////////////
		/// \brief Set the scrollbar's color
		///
		/// \param _color The color
		///
		////////////////////////////////////////////////////////////
		void setScrollbarColor(const sf::Color& _color);

		////////////////////////////////////////////////////////////
		/// \brief Set the resize strips's gradient colors
		///
		/// \param _c1 The left and upper corner's colors
		/// \param _c2 The lower right corner's color
		///
		////////////////////////////////////////////////////////////
		void setResizeStripGradients(const sf::Color& _c1,
		                             const sf::Color& _c2);

		////////////////////////////////////////////////////////////
		/// \brief Get the box's fill color
		///
		////////////////////////////////////////////////////////////
		const sf::Color& getColor() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the scrollbar's color
		///
		////////////////////////////////////////////////////////////
		const sf::Color& getScrollbarColor() const;

		////////////////////////////////////////////////////////////
		/// \brief Set the size
		///
		/// \param _size The size
		///
		////////////////////////////////////////////////////////////
		void setSize(const sf::Vector2f& _size);

		////////////////////////////////////////////////////////////
		/// \brief Set the position
		///
		/// \param _pos The position
		///
		////////////////////////////////////////////////////////////
		void setPosition(const sf::Vector2f& _pos);

		////////////////////////////////////////////////////////////
		/// \brief Get the size
		///
		////////////////////////////////////////////////////////////
		const sf::Vector2f& getSize() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the position
		///
		////////////////////////////////////////////////////////////
		const sf::Vector2f& getPosition() const;

		////////////////////////////////////////////////////////////
		/// \brief Set whether the InfoBox is resizable
		///
		/// \param _r The boolean value
		///
		////////////////////////////////////////////////////////////
		void setResizable(bool _r);

		////////////////////////////////////////////////////////////
		/// \brief Set whether the InfoBox is draggable
		///
		/// \param _d The boolean value
		///
		////////////////////////////////////////////////////////////
		void setDraggable(bool _d);

		//add comments
		void snapToTop();
		//add comments
		void snapToBottom();

	  private:
		////////////////////////////////////////////////////////////
		/// \brief Is the box being moused over?
		///
		////////////////////////////////////////////////////////////
		bool boxMousedOver() const;

		////////////////////////////////////////////////////////////
		/// \brief Is the resizing strip being moused over?
		///
		////////////////////////////////////////////////////////////
		bool resizeStripMousedOver() const;

		////////////////////////////////////////////////////////////
		/// \brief Sets the position of the message at the given index
		///
		/// This function appropriately sets the position of a message
		/// by taking into account all previous messages and how many
		/// lines they have.
		///
		/// \param _index The index [in m_messages]
		///
		////////////////////////////////////////////////////////////
		void positionMessage(int _index);

		////////////////////////////////////////////////////////////
		/// \brief Re-initialise the InfoBox
		///
		/// Ensures the views, positions, sizes, etc. of everything
		/// are properly initialised according to the InfoBox's size
		/// and position, and the window's size. It also takes in a
		/// parameter that - if set to false - can make it run sign-
		/// ificantly faster by skipping logic related to resizing.
		///
		/// \param _resized Whether the window was resized
		///
		////////////////////////////////////////////////////////////
		void reset(bool _resized);

		////////////////////////////////////////////////////////////
		/// \brief Get the effective width of the InfoBox
		///
		/// Gets the maximum width Message items can be.
		///
		////////////////////////////////////////////////////////////
		float getUsableWidth() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the distance between the first/last menu items
		///
		////////////////////////////////////////////////////////////
		float getMenuHeight() const;

		////////////////////////////////////////////////////////////
		/// \brief (Re)initialise scrollbar parameters as appropriate
		///
		////////////////////////////////////////////////////////////
		void setupScrollbar();

		////////////////////////////////////////////////////////////
		/// \brief Move the InfoBox if it's being dragged
		///
		////////////////////////////////////////////////////////////
		void dragBox();

		////////////////////////////////////////////////////////////
		/// \brief Resize the InfoBox if it's being resized
		///
		////////////////////////////////////////////////////////////
		void resizeBox();

		////////////////////////////////////////////////////////////
		/// \brief Handle InfoBox-related user input
		///
		////////////////////////////////////////////////////////////
		void detectBoxInteractions(sf::Event& _event);

		////////////////////////////////////////////////////////////
		/// \brief Handle resize strip-related user input
		///
		////////////////////////////////////////////////////////////
		void detectResizeStripInteractions(sf::Event& _event);

		///////////////////////////////////////////////////////////
		//Data members --------------------------------------------
		///////////////////////////////////////////////////////////
		mutable sf::RenderWindow*   m_window;              ///< The last window draw() is called on
		sf::Vector2u                m_lastWindowSize;      ///< The last known window size
		sf::Vector2f                m_size;                ///< The size of the box
		sf::Vector2f                m_position;            ///< The position of the box
		const sf::Font&             m_font;                ///< Font used to draw text
		unsigned                    m_charSize;            ///< The character size
		bool                        m_resizable;           ///< Is the box resizable?
		bool                        m_resizing;            ///< Is the box being resized?
		bool                        m_draggable;           ///< Is the box draggable?
		bool                        m_dragging;            ///< Is the box being dragged?
		sf::Vector2i                m_lastMousePosition;   ///< The last known mouse position
		sf::View                    m_view;                ///< The scrollable view
		sf::View                    m_shadedRectangleView; ///< The background shaded rectangle view
		sf::RectangleShape          m_shadedRectangle;     ///< The background shaded rectangle
		sf::VertexArray             m_resizeStrip;         ///< The triangle you click to resize the box
		std::vector<InfoBoxMessage> m_messages;            ///< The messages contained within the box
		spss::Scrollbar             m_scrollbar;           ///< The scrollbar
		                                                   //---------------------------------------------
	};
} // namespace spss

#endif // INFOBOX_H_INCLUDED
