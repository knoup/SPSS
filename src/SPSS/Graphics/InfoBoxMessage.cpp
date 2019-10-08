#include <SPSS/Graphics/InfoBoxMessage.h>

namespace spss {

	void removeNewlines(std::string& _str) {
		std::string newline{"\n"};
		auto        i = _str.find(newline);
		while (i != std::string::npos) {
			_str.erase(i, newline.length());
			i = _str.find(newline, i);
		}
	}

	////////////////////////////////////////////////////////////
	InfoBoxMessage::InfoBoxMessage(const Message&  _msg,
	                               const sf::Font& _font,
	                               unsigned int    _charSize)
	            : m_text{"", _font, _charSize},
	              m_message{_msg},
	              m_titleColor{sf::Color(100, 100, 100, 255)},
	              m_contentColor{sf::Color::White},
	              m_prefix{""},
	              m_suffix{":"},
	              m_prefixColor{sf::Color::White},
	              m_suffixColor{sf::Color::White} {
		setMessage(_msg);
	}

	////////////////////////////////////////////////////////////
	void InfoBoxMessage::draw(sf::RenderTarget& target,
	                          sf::RenderStates  states) const {
		target.draw(m_text, states);
	}

	////////////////////////////////////////////////////////////
	void InfoBoxMessage::fitWidth(float _width) {
		bool tooWide{m_text.getGlobalBounds().width >= _width};
		bool tooNarrow{m_text.getGlobalBounds().width < _width &&
		               getNumberOfLines() > 1};

		if (!tooWide && !tooNarrow) {
			return;
		}

		setTextString(getRawString());
		std::string textStr = getRawString();

		float lineWidth{0};

		for (size_t i{0}; i < textStr.length(); i++) {
			auto pos{m_text.findCharacterPos(i)};
			char charAtPos{textStr.at(i)};
			auto glyph{m_text.getFont()->getGlyph(charAtPos, m_text.getCharacterSize(), false, m_text.getOutlineThickness(i))};

			float charWidth{glyph.bounds.width};
			float spacing{glyph.advance};

			//Insert a newline at the prior position
			//if needed
			if (lineWidth + spacing >= _width) {
				textStr.insert(i, 1, '\n');
				lineWidth = 0;
				i++;
			}

			lineWidth += spacing;
		}

		setTextString(textStr);
	}

	////////////////////////////////////////////////////////////
	void InfoBoxMessage::setMessage(const Message& _msg, float _width) {
		m_message = _msg;
		removeNewlines(m_message.title);
		removeNewlines(m_message.content);
		if (_width > 0) {
			fitWidth(_width);
			return;
		}
		setTextString(getRawString());
	}

	////////////////////////////////////////////////////////////
	void InfoBoxMessage::setTitle(const std::string& _title, float _width) {
		m_message.title = _title;
		setMessage(m_message, _width);
	}

	////////////////////////////////////////////////////////////
	void InfoBoxMessage::setContent(const std::string& _content, float _width) {
		m_message.content = _content;
		setMessage(m_message, _width);
	}

	////////////////////////////////////////////////////////////
	void InfoBoxMessage::setTitleColor(const sf::Color& _color) {
		m_titleColor = _color;
		//Since changing the color changes nothing about
		//the geometry of the text object, we'll call
		//setTextString() and give it the current string,
		//with newlines and all, so we can refresh our
		//color changes
		setTextString(m_text.getString());
	}

	////////////////////////////////////////////////////////////
	void InfoBoxMessage::setContentColor(const sf::Color& _color) {
		m_contentColor = _color;
		//Since changing the color changes nothing about
		//the geometry of the text object, we'll call
		//setTextString() and give it the current string,
		//with newlines and all, so we can refresh our
		//color changes
		setTextString(m_text.getString());
	}

	////////////////////////////////////////////////////////////
	void InfoBoxMessage::setTitleAffixes(const std::string& _prefix,
	                                     const std::string& _suffix,
	                                     const sf::Color&   _prefixColor,
	                                     const sf::Color&   _suffixColor,
	                                     float              _width) {
		m_prefix      = _prefix;
		m_suffix      = _suffix;
		m_prefixColor = _prefixColor;
		m_suffixColor = _suffixColor;
		setMessage(m_message, _width);
	}

	////////////////////////////////////////////////////////////
	void InfoBoxMessage::setPosition(const sf::Vector2f& _pos) {
		m_text.setPosition(_pos);
	}

	////////////////////////////////////////////////////////////
	void InfoBoxMessage::setPosition(float _x, float _y) {
		setPosition({_x, _y});
	}

	////////////////////////////////////////////////////////////
	const spss::MulticolorText& InfoBoxMessage::getText() const {
		return m_text;
	}

	////////////////////////////////////////////////////////////
	const unsigned int InfoBoxMessage::getNumberOfLines() const {
		const std::string& s{m_text.getString()};
		return getNumberOfLines(0, s.length() - 1);
	}

	////////////////////////////////////////////////////////////
	const unsigned int InfoBoxMessage::getNumberOfLines(size_t _startPos,
	                                                    size_t _endPos) const {
		const std::string& s{m_text.getString()};
		int                count{1};

		//trueIndex will represent the actual index we're checking;
		//it will be incremented with each iteration.

		//index will only be incremented if a newline is NOT encout-
		//ered. This ensures that when given, say, 0 as startPos and
		//9 as endPos, any newlines in between those ranges won't count
		//towards the number of iterations, and thus we'll iterate over
		//exactly 10 non-newline characters.
		//
		//Of course, we'll also need to stop if trueIndex reaches the
		//end of the string.

		size_t trueIndex{_startPos};
		size_t index{_startPos};

		while (index <= _endPos && trueIndex < s.length()) {
			if (s.at(trueIndex) == '\n') {
				++count;
			}
			else {
				index++;
			}

			trueIndex++;
		}

		return count;
	}

	////////////////////////////////////////////////////////////
	const std::string InfoBoxMessage::getRawString() const {
		std::string s{""};
		s = (m_prefix + m_message.title + m_suffix + " ");
		s += m_message.content;
		return s;
	}

	////////////////////////////////////////////////////////////
	void InfoBoxMessage::setTextString(const std::string& _str) {
		m_text.setString(_str);
		//After changing the string, our fill/outline colours
		//and outline thicknesses are reset, so we'll reapply them
		m_text.setOutlineThickness(1);

		size_t prefixLength{m_prefix.length()};
		size_t suffixLength{m_suffix.length()};
		size_t titleLength{m_message.title.length()};
		size_t contentLength{m_message.content.length()};

		//If the string contains newlines, we'll need to identify in which
		//places they occur (prefix, title, suffix, and content) in order
		//to be able to properly get the indeces we need to set the fill
		//colors. Therefore, for each segment, we'll check how many newl-
		//ines there are, and append the result to the end index.
		//
		//Since getNumberOfLines() returns 1 normally, we'll decrement
		//each offset value by 1.
		//
		//There are also checks to ensure the end indeces (size_t)
		//don't overflow by being decremented below 0, and to make
		//sure each begin index starts one index greater than where
		//the last one ended (unless the last one had a length of 0)

		//prefix
		//-------------------------------
		size_t prefixBegin{0};
		size_t prefixEnd{prefixBegin};
		if (prefixLength > 1) {
			prefixEnd += (prefixLength - 1);
		}
		auto prefixOffset{getNumberOfLines(prefixBegin, prefixEnd) - 1};
		prefixEnd += prefixOffset;

		//title
		//-------------------------------
		size_t titleBegin{prefixEnd};
		if (prefixLength > 0) {
			titleBegin += 1;
		}
		size_t titleEnd{titleBegin};
		if (titleLength > 1) {
			titleEnd += (titleLength - 1);
		}
		auto titleOffset{getNumberOfLines(titleBegin, titleEnd) - 1};
		titleEnd += titleOffset;

		//suffix
		//-------------------------------
		size_t suffixBegin{titleEnd};
		if (titleLength > 0) {
			suffixBegin += 1;
		}
		size_t suffixEnd{suffixBegin};
		if (suffixLength > 1) {
			suffixEnd += (suffixLength - 1);
		}
		auto suffixOffset{getNumberOfLines(suffixBegin, suffixEnd) - 1};
		suffixEnd += suffixOffset;

		//content
		//-------------------------------
		//+2 because of the space that comes after the title's suffix
		//(see getRawString())
		size_t contentBegin{suffixEnd + 1};
		if (suffixLength > 0) {
			contentBegin += 1;
		}
		size_t contentEnd{contentBegin};
		if (contentLength > 1) {
			contentEnd += (contentLength - 1);
		}
		auto contentOffset{getNumberOfLines(contentBegin, contentEnd) - 1};
		contentEnd += contentOffset;

		m_text.setFillColor(m_prefixColor, prefixBegin, prefixEnd);
		m_text.setFillColor(m_titleColor, titleBegin, titleEnd);
		m_text.setFillColor(m_suffixColor, suffixBegin, suffixEnd);
		m_text.setFillColor(m_contentColor, contentBegin, contentEnd);
	}
} // namespace spss
