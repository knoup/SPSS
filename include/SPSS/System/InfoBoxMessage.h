#ifndef INFOBOXMESSAGE_H_INCLUDED
#define INFOBOXMESSAGE_H_INCLUDED

////////////////////////////////////////////////////////////////////
/// spss::InfoBoxMessage is a wrapper around spss::MulticolorText.
/// It's meant to represent a text object in the Title: Content for-
/// mat, and has wrapping functionality, so it can automatically
/// insert line breaks as needed to fit a user-specified width.
////////////////////////////////////////////////////////////////////

#include <SPSS/Graphics/MulticolorText.h>
#include <SPSS/Util/Message.h>

#include <SFML/Graphics.hpp>

namespace spss {

	class InfoBoxMessage : public sf::Drawable {

	  public:
	  	////////////////////////////////////////////////////////////
		/// \brief Construct the InfoBoxMessage
		///
		/// \param _msg      The message
		/// \param _font     Font used to draw text
		/// \param _charSize The character size
		///
		////////////////////////////////////////////////////////////
		InfoBoxMessage(const Message&  _msg,
					   const sf::Font& _font,
					   unsigned int    _charSize);

		////////////////////////////////////////////////////////////
		/// \brief Draw the text to a render target
		///
		/// \param target Render target to draw to
		/// \param states Current render states
		///
		////////////////////////////////////////////////////////////
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		////////////////////////////////////////////////////////////
		/// \brief Ensure the text's width doesn't exceed a value
		///
		/// This function inserts line breaks as neccessary to
		/// keep the width of the text object within a given
		/// value
		///
		/// \param _width The maximum width this text can take up
		///
		////////////////////////////////////////////////////////////
		void fitWidth(float _width);

		////////////////////////////////////////////////////////////
		/// \brief Set a new message to be displayed
		///
		/// Since modifying the message can change the text's hori-
		/// zontal size, an optional width parameter can be given.
		/// This is simply a shortcut to calling fitWidth().
		///
		/// \param _msg   The new message
		/// \param _width The maximum width this text can take up
		///
		////////////////////////////////////////////////////////////
		void setMessage(const Message& _msg, float _width = 0);

		////////////////////////////////////////////////////////////
		/// \brief Set a new message title to be displayed
		///
		/// Since modifying the message can change the text's hori-
		/// zontal size, an optional width parameter can be given.
		/// This is simply a shortcut to calling fitWidth().
		///
		/// \param _title The new message title
		/// \param _width The maximum width this text can take up
		///
		////////////////////////////////////////////////////////////
		void setTitle(const std::string& _title, float _width = 0);

		////////////////////////////////////////////////////////////
		/// \brief Set a new message content to be displayed
		///
		/// Since modifying the message can change the text's hori-
		/// zontal size, an optional width parameter can be given.
		/// This is simply a shortcut to calling fitWidth().
		///
		/// \param _content The new message content
		/// \param _width   The maximum width this text can take up
		///
		////////////////////////////////////////////////////////////
		void setContent(const std::string& _content, float _width = 0);

		////////////////////////////////////////////////////////////
		/// \brief Set's the title's fill color
		///
		/// \param _color The color
		///
		////////////////////////////////////////////////////////////
		void setTitleColor(sf::Color _color);

		////////////////////////////////////////////////////////////
		/// \brief Set's the content's fill color
		///
		/// \param _color The color
		///
		////////////////////////////////////////////////////////////
		void setContentColor(sf::Color _color);

		////////////////////////////////////////////////////////////
		/// \brief Set's the title's affix values and colors
		///
		/// Allows the user to set the title's prefix and suffixes.
		/// For example, <Like this>. Or maybe ((Like this)). Colors
		/// for each can also be specified.
		///
		/// Since modifying the message can change the text's hori-
		/// zontal size, an optional width parameter can be given.
		/// This is simply a shortcut to calling fitWidth().
		///
		/// \param _prefix      The prefix value
		/// \param _suffix      The suffix value
		/// \param _prefixColor The prefix's color
		/// \param _suffixColor The suffix's color
		/// \param _width       The max width this text can take up
		///
		////////////////////////////////////////////////////////////
		void setTitleAffixes(const std::string& _prefix,
							 const std::string& _suffix,
							 sf::Color          _prefixColor = sf::Color::White,
							 sf::Color          _suffixColor = sf::Color::White,
							 float              _width = 0);

		////////////////////////////////////////////////////////////
		/// \brief Set the text's position
		///
		/// \param _pos The position
		///
		////////////////////////////////////////////////////////////
		void setPosition(sf::Vector2f _pos);

		////////////////////////////////////////////////////////////
		/// \brief Set the text's position
		///
		/// \param _pos The position
		///
		////////////////////////////////////////////////////////////
		void setPosition(float _x, float _y);

		////////////////////////////////////////////////////////////
		/// \brief Get the text
		///
		////////////////////////////////////////////////////////////
		const spss::MulticolorText& getText() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the number of lines the text spans
		///
		/// This is the public version of the below function. It
		/// simply gets the number of lines the entire text takes
		/// up, instead of from/to a specific position.
		///
		////////////////////////////////////////////////////////////
		const unsigned int getNumberOfLines() const;

	  private:
	  	////////////////////////////////////////////////////////////
		/// \brief Get the number of lines the text spans
		///
		/// Note that the start and end positions don't include any
		/// '\n' characters. I.E., this function will always itera-
		/// te over (_endPos - _startPos) + 1 NON-newline characters.
		/// For example, given the following string:
		///
		/// t e s t s t \n r i n g
		/// | | | | | |  | | | | |
		/// 0 1 2 3 4 5  6 7 8 9 10
		///
		/// and given a startPos of 0 and endPos of 7, this function
		/// will go from 0 (t) to 8 (i), because the \n at 6 isn't
		/// counted. It will also stop if it reaches the end of the
		/// string.
		///
		/// \param _startPos The starting index
		/// \param _endPos   The ending index
		///
		////////////////////////////////////////////////////////////
	  	const unsigned int getNumberOfLines(size_t _startPos,
									        size_t _endPos) const;


		////////////////////////////////////////////////////////////
		/// \brief Get the plain, unaltered string
		///
		/// Gets prefix + title + suffix + ' ' + content, with no
		/// newlines.
		///
		////////////////////////////////////////////////////////////
	  	const std::string getRawString() const;

	  	////////////////////////////////////////////////////////////
		/// \brief Sets m_text's string
		///
		/// Sets m_text's string to the given argument, and then
		/// makes sure everything (pre/suffix, title, content) are
		/// colored as they should be.
		///
		/// \param _str The final (with newlines) string to be set
		////////////////////////////////////////////////////////////
	  	void setTextString(const std::string& _str);

		///////////////////////////////////////////////////////////
		//Data members --------------------------------------------
		///////////////////////////////////////////////////////////
		spss::MulticolorText m_text;         ///< The text object
		Message              m_message;      ///< The message
		sf::Color            m_titleColor;   ///< The title color
		sf::Color            m_contentColor; ///< The content color
		std::string          m_prefix;       ///< The title prefix
		std::string          m_suffix;       ///< The title suffix
		sf::Color            m_prefixColor;  ///< The title prefix's color
		sf::Color            m_suffixColor;  ///< The title suffix's color
											 //---------------------------------------------
	};
} // namespace spss

#endif // INFOBOXMESSAGE_H_INCLUDED
