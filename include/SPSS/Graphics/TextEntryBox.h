#ifndef SPSS_TEXTENTRYBOX_H_INCLUDED
#define SPSS_TEXTENTRYBOX_H_INCLUDED

////////////////////////////////////////////////////////////////////
///
/// spss::TextEntryBox represents just that - a box in which text
/// can be entered.
///
/// It comes with all the functionality one would expect a text entry
/// box to have - including keyboard shortcuts for the most frequently
/// utilised functions (selection, skipping words, copying, pasting,
/// etc.).
///
/// The text the user types is of the customised spss::MulticolorText
/// type, which removes sf::Text's limitations of being able to only
/// set one color for the entire text object. An spss::TextEntryBox
/// object will thus automatically make text that overflows out of the
/// box transparent.
///
////////////////////////////////////////////////////////////////////

#include <SPSS/Graphics/MulticolorText.h>
#include <SFML/Graphics.hpp>

namespace spss {

	class TextEntryBox : public sf::Drawable {
	  public:
		////////////////////////////////////////////////////////////
		/// \brief Construct the text entry box
		///
		/// \param _font     Font used to draw text
		/// \param _width    The width of the box
		/// \param _str      The default string value
		///
		////////////////////////////////////////////////////////////
		TextEntryBox(const sf::Font&    _font,
		             float              _width,
		             const std::string& _str = "");

		////////////////////////////////////////////////////////////
		/// \brief Set the position of the text entry box
		///
		/// \param _x   The x coordinate
		/// \param _y   The y coordinate
		///
		////////////////////////////////////////////////////////////
		void setPosition(float _x, float _y);

		////////////////////////////////////////////////////////////
		/// \brief Set the position of the text entry box
		///
		/// \param _position The position
		///
		////////////////////////////////////////////////////////////
		void setPosition(const sf::Vector2f& _position);

		////////////////////////////////////////////////////////////
		/// \brief Set the color of the box
		///
		/// \param _color The color
		///
		/// \see getBackgroundColor
		///
		////////////////////////////////////////////////////////////
		void setBackgroundColor(const sf::Color& _color);

		////////////////////////////////////////////////////////////
		/// \brief Set the color of the caret
		///
		/// \param _color The color
		///
		/// \see getCaretColor
		///
		////////////////////////////////////////////////////////////
		void setCaretColor(const sf::Color& _color);

		////////////////////////////////////////////////////////////
		/// \brief Set the text's fill color
		///
		/// \param _color The color
		///
		/// \see getFillColor
		///
		////////////////////////////////////////////////////////////
		void setFillColor(const sf::Color& _c);

		////////////////////////////////////////////////////////////
		/// \brief Set the text's outline color
		///
		/// \param _color The color
		///
		/// \see getOutlineColor
		///
		////////////////////////////////////////////////////////////
		void setOutlineColor(const sf::Color& _c);

		////////////////////////////////////////////////////////////
		/// \brief Set the text's outline thickness
		///
		/// \param _f The thickness
		///
		/// \see getOutlineThickness
		///
		////////////////////////////////////////////////////////////
		void setOutlineThickness(float _f);

		////////////////////////////////////////////////////////////
		/// \brief Get the box's color
		///
		/// \see setBackgroundColor
		///
		////////////////////////////////////////////////////////////
		const sf::Color getBackgroundColor() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the caret's color
		///
		/// \see setCaretColor
		///
		////////////////////////////////////////////////////////////
		const sf::Color getCaretColor() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the text's fill color
		///
		/// \see setFillColor
		///
		////////////////////////////////////////////////////////////
		const sf::Color getFillColor() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the text's outline color
		///
		/// \see setOutlineColor
		///
		////////////////////////////////////////////////////////////
		const sf::Color getOutlineColor() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the text's outline thickness
		///
		/// \see setOutlineThickness
		///
		////////////////////////////////////////////////////////////
		float getOutlineThickness() const;

		////////////////////////////////////////////////////////////
		/// \brief Set the text's character size
		///
		/// \param _i The character size
		///
		////////////////////////////////////////////////////////////
		void setCharSize(unsigned int _i);

		////////////////////////////////////////////////////////////
		/// \brief Set the maximum amount of characters
		///
		/// \param _i The maximum amount of characters
		///
		////////////////////////////////////////////////////////////
		void setMaxChars(unsigned int _i);

		////////////////////////////////////////////////////////////
		/// \brief Toggle always-on visibility
		///
		/// If true, the text entry box will be drawn even if no
		/// text is being entered
		///
		/// \param _b The boolean value
		///
		////////////////////////////////////////////////////////////
		void setAlwaysVisible(bool _b);

		////////////////////////////////////////////////////////////
		/// \brief Toggle always-on activeness
		///
		/// If true, the text entry box will be always active and
		/// will constantly be getting text input. Additionally,
		/// text will not be cleared upon pressing RETURN.
		///
		/// \param _b The boolean value
		///
		////////////////////////////////////////////////////////////
		void setAlwaysActive(bool _b);

		////////////////////////////////////////////////////////////
		/// \brief Set the width of the text entry box
		///
		/// The height will be automatically determined depending on
		/// the character size.
		///
		/// \param _width The desired width
		///
		/// \see getWidth
		///
		////////////////////////////////////////////////////////////
		void setWidth(float _width);

		////////////////////////////////////////////////////////////
		/// \brief Gets the width of the text entry box
		///
		/// \see setWidth
		///
		////////////////////////////////////////////////////////////
		float getWidth() const;

		////////////////////////////////////////////////////////////
		/// \brief Gets the height of the text entry box
		///
		////////////////////////////////////////////////////////////
		float getHeight() const;

		////////////////////////////////////////////////////////////
		/// \brief Gets the font's line spacing value
		///
		////////////////////////////////////////////////////////////
		float getLineSpacing() const;

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
		/// \brief Draw the text to a render target
		///
		/// sf::Drawable overload.
		///
		/// \param target RenderTarget to draw to
		/// \param states Current render states
		///
		////////////////////////////////////////////////////////////
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		////////////////////////////////////////////////////////////
		/// \brief Is text being entered?
		///
		/// Returns whether of not the text box is active, i.e.,
		/// accepting user input.
		///
		////////////////////////////////////////////////////////////
		bool enteringText() const;

		////////////////////////////////////////////////////////////
		/// \brief Is the user's text input complete?
		///
		/// Returns true after a user has finished entering text.
		/// The last entered string can be retrieved via
		/// getLastString
		///
		/// \see getLastString
		///
		////////////////////////////////////////////////////////////
		bool inputComplete() const;

		////////////////////////////////////////////////////////////
		/// \brief Returns the last string a user has finished inputting
		///
		/// After RETURN is pressed, the contents of the text entry box
		/// are cleared and moved over to m_lastString. It can be retr-
		/// ieved via this function.
		///
		////////////////////////////////////////////////////////////
		const std::string getLastString() const;

		////////////////////////////////////////////////////////////
		/// \brief Returns the current contents of the text entry box
		///
		////////////////////////////////////////////////////////////
		const std::string getCurrentString() const;

		////////////////////////////////////////////////////////////
		/// \brief Toggles whether the text entry box is active
		///
		/// When set to true, the text entry box will accept input.
		///
		////////////////////////////////////////////////////////////
		void setActive(bool _b);

	  private:
		////////////////////////////////////////////////////////////
		/// \brief Handles user input given the unicode code.
		///
		/// Depending on whether a sequence is selected, or if the
		/// key pressed is the backspace key, this function may call
		/// deleteSelection before insert
		///
		/// \see deleteSelection
		/// \see insert
		///
		////////////////////////////////////////////////////////////
		void handleInput(sf::Uint32 _unicode);

		////////////////////////////////////////////////////////////
		/// \brief Sets the text position.
		///
		/// Sets the text's position and then calls updateCaret,
		/// updateHighlight, and updateTransparency
		///
		/// \see updateCaret
		/// \see updateHighlight
		/// \see updateTransparency
		///
		////////////////////////////////////////////////////////////
		void setTextPosition(const sf::Vector2f& _pos);

		////////////////////////////////////////////////////////////
		/// \brief Sets the text's string
		///
		/// Since MulticolorText will lose its outline thickness
		/// when changing its string, we'll use this function to
		/// reset it every time we change the string. Also calls
		/// shiftTextToRight.
		///
		/// \see setOutlineThickness
		/// \see shiftTextToRight
		///
		////////////////////////////////////////////////////////////
		void setTextString(const std::string& _str);

		////////////////////////////////////////////////////////////
		/// \brief Updates the transparency of overflowing text
		///
		/// Checks for letters that are outside of the bounds of
		/// the box and makes them transparent.
		///
		////////////////////////////////////////////////////////////
		void updateTransparency();

		////////////////////////////////////////////////////////////
		/// \brief Updates caret's position and transparency value
		///
		/// Ensures that the caret's position is where it should be
		/// according to the user's selection. Also manages the caret's
		/// fading in/out effect.
		///
		///////////////////////////////////////////////////////////
		void updateCaret();

		////////////////////////////////////////////////////////////
		/// \brief Updates the highlighted text's rectangle
		///
		/// Manages the position and size of the selection rectangle
		/// according to the user's selection and the bounds of the box.
		///
		///////////////////////////////////////////////////////////
		void updateHighlight();

		////////////////////////////////////////////////////////////
		/// \brief Are the contents of the text entry box blank?
		///
		///////////////////////////////////////////////////////////
		bool stringEmpty() const;

		////////////////////////////////////////////////////////////
		/// \brief Selects all the contents of the text entry box
		///
		///////////////////////////////////////////////////////////
		void selectAll();

		////////////////////////////////////////////////////////////
		/// \brief Unselects all the contents of the text entry box
		///
		///////////////////////////////////////////////////////////
		void unselectAll();

		////////////////////////////////////////////////////////////
		/// \brief Moves the caret one step to the left
		///
		///////////////////////////////////////////////////////////
		void moveLeft();

		////////////////////////////////////////////////////////////
		/// \brief Moves the caret one step to the right
		///
		///////////////////////////////////////////////////////////
		void moveRight();

		////////////////////////////////////////////////////////////
		/// \brief Selects one step to the left
		///
		/// Selects the text from the caret's position to one step
		/// to the left
		///
		///////////////////////////////////////////////////////////
		void selectLeft();

		////////////////////////////////////////////////////////////
		/// \brief Selects one step to the right
		///
		/// Selects the text from the caret's position to one step
		/// to the right
		///
		///////////////////////////////////////////////////////////
		void selectRight();

		////////////////////////////////////////////////////////////
		/// \brief Shifts text to the left if neccessary
		///
		/// If the caret goes out of the right edge of the box, this
		/// function shifts the position of m_text to the left as much
		/// as neccessary to ensure that the caret is still visible.
		/// Note that text out of bounds of the box will become trans-
		/// parent.
		///
		/// \see updateTransparency
		///
		///////////////////////////////////////////////////////////
		void shiftTextToLeft();

		////////////////////////////////////////////////////////////
		/// \brief Shifts text to the right if neccessary
		///
		/// If the caret goes out of the left edge of the box, this
		/// function shifts the position of m_text to the right as much
		/// as neccessary to ensure that the caret is still visible.
		/// Note that text out of bounds of the box will become trans-
		/// parent.
		///
		/// \see updateTransparency
		///
		///////////////////////////////////////////////////////////
		void shiftTextToRight();

		////////////////////////////////////////////////////////////
		/// \brief Resets the text's position
		///
		/// Sets the text's X position to the box's (m_rectangle) X
		/// position.
		///
		/// \see setTextPosition
		///
		///////////////////////////////////////////////////////////
		void resetTextPosition();

		////////////////////////////////////////////////////////////
		/// \brief Returns the position of the start of the last word
		///
		/// Used when holding CTRL and "skipping" words
		///
		///////////////////////////////////////////////////////////
		size_t posAtPreviousWord() const;

		////////////////////////////////////////////////////////////
		/// \brief Returns the position of the end of the next word
		///
		/// Used when holding CTRL and "skipping" words
		///
		///////////////////////////////////////////////////////////
		size_t posAtNextWord() const;

		////////////////////////////////////////////////////////////
		/// \brief Are multiple letters selected?
		///
		///////////////////////////////////////////////////////////
		bool sequenceSelected() const;

		////////////////////////////////////////////////////////////
		/// \brief Delete the entirety of the current selection
		///
		///////////////////////////////////////////////////////////
		void deleteSelection();

		////////////////////////////////////////////////////////////
		/// \brief Get the contents of the current selection
		///
		///////////////////////////////////////////////////////////
		const std::string getSelection() const;

		////////////////////////////////////////////////////////////
		/// \brief Is the input valid?
		///
		/// Anything between 32 and 255 represents the  alphanumeric
		/// characters, and some symbols.
		///
		/// \see insert
		///
		///////////////////////////////////////////////////////////
		bool validInsertion(sf::Uint32 _unicode) const;

		////////////////////////////////////////////////////////////
		/// \brief Insert the string into the currently selected position
		///
		///////////////////////////////////////////////////////////
		void insert(std::string& _str);

		////////////////////////////////////////////////////////////
		/// \brief Insert the char into the currently selected position
		///
		///////////////////////////////////////////////////////////
		void insert(char _char);

		////////////////////////////////////////////////////////////
		/// \brief Clear all the contents of the text entry box
		///
		///////////////////////////////////////////////////////////
		void clearText();

		///////////////////////////////////////////////////////////
		//Data members --------------------------------------------
		///////////////////////////////////////////////////////////

		const sf::Font&      m_font;                 ///< Font used to display the title and menu items
		sf::RectangleShape   m_rectangle;            ///< The background rectangle
		sf::RectangleShape   m_highlightedRectangle; ///< The rectangle used to highlight the current selection
		size_t               m_selectionBegin;       ///< The beginning index of the current selection
		size_t               m_selectionEnd;         ///< The end index of the current selection
		int                  m_selectionDirection;   ///< What direction the user is selecting text in (-1 => left, 0 => neutral, 1 => right)
		spss::MulticolorText m_text;                 ///< The text used to display what the user is entering
		sf::Text             m_caret;                ///< The caret
		bool                 m_enteringText;         ///< Is the text entry box active, i.e. accepting input?
		mutable bool         m_inputComplete;        ///< Did the user finish entering text (press RETURN)?
		bool                 m_alwaysVisible;        ///< Display the box at all times, even when m_enteringText is false?
		bool                 m_alwaysActive;         ///< m_enteringText is always set to true and prevent text from being cleared upon pressing RETURN?
		std::string          m_lastString;           ///< The last string entered by the user (RETURN was pressed)
		unsigned int         m_charSize;             ///< m_text's character size
		unsigned int         m_maxChars;             ///< The maximum amount of characters that can be entered
		sf::Color            m_fillColor;            ///< m_text's fill color
		sf::Color            m_outlineColor;         ///< m_text's outline color
		float                m_outlineThickness;     ///< m_text's outline thickness
	};

} //namespace spss

#endif // SPSS_TEXTENTRYBOX_H_INCLUDED
