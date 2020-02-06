#pragma once

////////////////////////////////////////////////////////////////////
/// spss::DialogPrompt is a draggable box which can be used for
/// things like notifications.
///
/// An indefinite amount of buttons, with an associated function,
/// can be added. Optionally, a text entry box can be enabled as
/// well. This makes DialogPrompt useful for getting text input
/// and/or performing a specific action upon making a selection.
///
/// Size is automatically determined and depends on the width of
/// the title text, as well as the number of buttons. For this
/// reason, DialogPrompt's origin is always automatically centered.
////////////////////////////////////////////////////////////////////

#include <any>

#include <SPSS/Graphics/DrawableToWindow.h>
#include <SPSS/Graphics/TextEntryBox.h>
#include <SPSS/Util/Function.h>

namespace spss {
	class DialogPrompt : public spss::DrawableToWindow {
	  public:
		////////////////////////////////////////////////////////////
		/// \brief Construct the DialogPrompt
		///
		/// \param _textEntryEnabled  Whether text entry is enabled
		/// \param _position          The position (in pixels)
		/// \param _font              Font used to draw text
		/// \param _promptTitle       The prompt title's default string
		/// \param _titleCharSize     The prompt title's character size
		/// \param _boxDefaultStr     The text entry box's default string
		/// \param _boxCharSize       The text entry box's character size
		///
		////////////////////////////////////////////////////////////
		DialogPrompt(bool                _textEntryEnabled,
		             const sf::Vector2f& _position,
		             const sf::Font&     _font,
		             const std::string&  _promptTitle,
		             const unsigned int  _titleCharSize = 20,
		             const std::string&  _boxDefaultStr = "",
		             const unsigned int  _boxCharSize   = 20);

		////////////////////////////////////////////////////////////
		/// \brief Get input
		///
		/// \param _e A reference to a captured event
		///
		////////////////////////////////////////////////////////////
		void getInput(sf::Event& _e);

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
		/// \brief Adds a clickable button
		///
		/// \param _str    The button's text
		/// \param _action The function called upon click
		///
		////////////////////////////////////////////////////////////
		void addButton(const std::string& _str, spss::Function<std::any> _action);

		////////////////////////////////////////////////////////////
		/// \brief Gets the text entry box's current string
		///
		/// If text entry is disabled, returns a blank string
		///
		////////////////////////////////////////////////////////////
		const std::string getString() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the position
		///
		////////////////////////////////////////////////////////////
		const sf::Vector2f& getPosition() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the size
		///
		////////////////////////////////////////////////////////////
		const sf::Vector2f& getSize() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the local bounds
		///
		////////////////////////////////////////////////////////////
		const sf::FloatRect& getLocalBounds() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the global bounds
		///
		////////////////////////////////////////////////////////////
		const sf::FloatRect& getGlobalBounds() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the color
		///
		////////////////////////////////////////////////////////////
		const sf::Color& getColor() const;

		////////////////////////////////////////////////////////////
		/// \brief Set the position
		///
		/// \param _pos The position
		///
		////////////////////////////////////////////////////////////
		void setPosition(const sf::Vector2f& _pos);

		////////////////////////////////////////////////////////////
		/// \brief Set the color
		///
		/// \param _color The color
		///
		////////////////////////////////////////////////////////////
		void setColor(const sf::Color& _color);

		////////////////////////////////////////////////////////////
		/// \brief Set the text entry's input limit, if enabled
		///
		/// \param _i The char limit
		///
		////////////////////////////////////////////////////////////
		void setMaxChars(unsigned int _i);

		////////////////////////////////////////////////////////////
		/// \brief Set whether the prompt is draggable
		///
		/// \param _d Is it draggable?
		///
		////////////////////////////////////////////////////////////
		void setDraggable(bool _d);

	  private:
		////////////////////////////////////////////////////////////
		/// This struct will represent the clickable buttons
		///
		struct Button {
			sf::RectangleShape       m_shape;
			sf::Text                 m_text;
			spss::Function<std::any> m_action;

			void setPosition(const sf::Vector2f& _pos) {
				m_shape.setPosition(_pos);
				m_text.setPosition(_pos);
			}
		};
		////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////
		/// \brief Add/remove breaks as necessary to fit a given width
		///
		/// Ensures that the title text fits the given width. If it's
		/// too wide, '\n' characters will be inserted where needed.
		/// If there are redundant linebreaks, they will be removed.
		///
		/// \param _width The width
		///
		////////////////////////////////////////////////////////////
		void fitWidth(float _width);

		////////////////////////////////////////////////////////////
		/// \brief Set the dialog prompt's height
		///
		/// \param _height The height
		///
		////////////////////////////////////////////////////////////
		void setHeight(float _height);

		////////////////////////////////////////////////////////////
		/// \brief Set the dialog prompt's width
		///
		/// \param _width The width
		///
		////////////////////////////////////////////////////////////
		void setWidth(float _width);

		////////////////////////////////////////////////////////////
		/// \brief Handle mouseclick-related input
		///
		////////////////////////////////////////////////////////////
		void handleMouseClick();

		////////////////////////////////////////////////////////////
		/// \brief Handle mouseover-related input
		///
		////////////////////////////////////////////////////////////
		void handleMouseover();

		////////////////////////////////////////////////////////////
		/// \brief Handles the dragging of the dialog prompt
		///
		////////////////////////////////////////////////////////////
		void dragBox();

		////////////////////////////////////////////////////////////
		/// \brief Positions all the prompt's contents appropriately
		///
		////////////////////////////////////////////////////////////
		void alignElements();

		mutable sf::RenderWindow*     m_window;            ///< The last window draw() is called on
		const sf::Font&               m_font;              ///< Font used to draw text
		sf::Text                      m_title;             ///< The dialog prompt's title text
		sf::Vector2i                  m_lastMousePosition; ///< The last known mouse position (used to determine offset when dragging)
		sf::Vector2f                  m_lastPosition;      ///< The last known prompt position (used to determine whether re-alignment is needed)
		bool                          m_draggable;         ///< Whether the prompt can be dragged
		bool                          m_dragging;          ///< Whether the prompt is being dragged
		sf::RectangleShape            m_rect;              ///< The prompt's main rectangle shape
		std::unique_ptr<TextEntryBox> m_textEntry;         ///< The [optional] text entry field
		std::vector<Button>           m_buttons;           ///< The buttons
		bool                          m_alignmentNeeded;   ///< Whether elements need to be re-aligned
	};
} // namespace spss
