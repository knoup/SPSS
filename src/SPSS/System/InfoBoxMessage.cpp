#include <SPSS/System/InfoBoxMessage.h>

namespace spss {

	InfoBoxMessage::InfoBoxMessage(const Message&  _msg,
	                                 const sf::Font& _font,
	                                 unsigned int    _charSize)
	            : m_text{}, m_message{_msg} {
		m_text.setFont(_font);
		m_text.setCharacterSize(_charSize);

		std::string finalStr{""};
		if (m_message.title != "") {
			finalStr = ("<" + m_message.title + "> ");
		}
		finalStr += m_message.content;
		setString(finalStr);
	}

	void InfoBoxMessage::removeNewlines(std::string& _str) {
		std::string newline{"\n"};
		auto        i = _str.find(newline);
		while (i != std::string::npos) {
			_str.erase(i, newline.length());
			i = _str.find(newline, i);
		}
	}

	void InfoBoxMessage::draw(sf::RenderTarget& target,
	                           sf::RenderStates  states) const {
		target.draw(m_text, states);
	}

	void InfoBoxMessage::fitWidth(float _width) {
		bool tooWide{m_text.getGlobalBounds().width >= _width};
		bool tooNarrow{m_text.getGlobalBounds().width < _width &&
		               getNumberOfLines() > 1};

		if (!tooWide && !tooNarrow) {
			return;
		}

		std::string textStr = m_text.getString();
		removeNewlines(textStr);
		setString(textStr);

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

		setString(textStr);
	}

	void InfoBoxMessage::setString(const std::string& _str) {
		m_text.setString(_str);

		//After changing the string, our fill/outline colours
		//and outline thicknesses are reset, so we'll reapply them

		m_text.setOutlineThickness(1);

		//If m_sender contains a newline, we'll want to make sure that
		//the whole name gets its colour set. Therefore, we'll check how
		//many newlines there are in the name part and make sure to add
		//the result to the end position of setFillColor().
		//Since getNumberOfLines returns 1 normally, we'll decrement our
		//initial result.
		unsigned int endOffset{getNumberOfLines(1, m_message.title.length())};
		--endOffset;

		if (m_message.title == "Server") {
			m_text.setFillColor(
			  sf::Color::Red, 1, m_message.title.length() + endOffset);
		}
		else if (m_message.title == "LocalPlayer") {
			m_text.setFillColor(
			  sf::Color::Cyan, 1, m_message.title.length() + endOffset);
		}
		else {
			m_text.setFillColor(
			  sf::Color::Magenta, 1, m_message.title.length() + endOffset);
		}
	}

	void InfoBoxMessage::setPosition(sf::Vector2f _pos) {
		m_text.setPosition(_pos);
	}

	void InfoBoxMessage::setTransparency(int _a) {
		m_text.setTransparency(_a);
	}

	sf::Vector2f InfoBoxMessage::getPosition() const {
		return m_text.getPosition();
	}

	sf::FloatRect InfoBoxMessage::getGlobalBounds() const {
		return m_text.getGlobalBounds();
	}

	sf::FloatRect InfoBoxMessage::getLocalBounds() const {
		return m_text.getLocalBounds();
	}

	int InfoBoxMessage::getTransparency() const {
		return m_text.getFillColor(0).a;
	}

	unsigned int InfoBoxMessage::getNumberOfLines(size_t _startPos,
	                                               size_t _endPos) const {
		const std::string& s{m_text.getString()};
		if (_endPos == 0) {
			_endPos = s.length() - 1;
		}
		std::string sub{s.substr(_startPos, _endPos)};

		int    count{1};
		size_t nPos = sub.find("\n");
		while (nPos != std::string::npos) {
			count++;
			nPos = sub.find("\n", nPos + 1);
		}

		return count;
	}
} // namespace spss
