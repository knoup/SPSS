#include <SPSS/Graphics/TextEntryBox.h>

#include <iostream>

constexpr sf::Keyboard::Key LCTRL            = sf::Keyboard::LControl;
constexpr sf::Keyboard::Key LSHIFT           = sf::Keyboard::LShift;
constexpr sf::Keyboard::Key TEXT_SELECTALL   = sf::Keyboard::A;
constexpr sf::Keyboard::Key TEXT_SELECTLEFT  = sf::Keyboard::Left;
constexpr sf::Keyboard::Key TEXT_SELECTRIGHT = sf::Keyboard::Right;
constexpr sf::Keyboard::Key TEXT_MOVELEFT    = sf::Keyboard::Left;
constexpr sf::Keyboard::Key TEXT_MOVERIGHT   = sf::Keyboard::Right;
constexpr sf::Keyboard::Key TEXT_COPY        = sf::Keyboard::C;
constexpr sf::Keyboard::Key TEXT_CUT         = sf::Keyboard::X;
constexpr sf::Keyboard::Key TEXT_PASTE       = sf::Keyboard::V;
constexpr sf::Keyboard::Key TEXT_RETURN      = sf::Keyboard::Return;

const bool keyPressed(sf::Keyboard::Key _key) {
	return sf::Keyboard::isKeyPressed(_key);
}

const bool keysPressedTogether(const std::vector<sf::Keyboard::Key>& _keys) {
	bool allPressed{true};
	for (const auto& key : _keys) {
		if (!keyPressed(key)) {
			allPressed = false;
			break;
		}
	}
	return allPressed;
}

namespace spss {

	////////////////////////////////////////////////////////////
	TextEntryBox::TextEntryBox(const sf::Font&    _font,
	                           unsigned int       _charSize,
	                           float              _width,
	                           const std::string& _str)
	            : m_font{_font},
	              m_rectangle(),
	              m_selectionBegin(0),
	              m_selectionEnd(0),
	              m_selectionDirection(SELDIR::NEUTRAL),
	              m_text(),
	              m_enteringText{false},
	              m_inputComplete{false},
	              m_alwaysVisible{false},
	              m_alwaysActive{false},
	              m_lastString{},
	              m_charSize{_charSize},
	              m_maxChars{255},
	              m_fillColor{sf::Color::White},
	              m_outlineColor{sf::Color::Black},
	              m_outlineThickness{0},
	              m_alphaUpdateNeeded{false},
	              m_xOffset{0.F} {
		setTextString(_str);

		m_rectangle.setFillColor(sf::Color(0, 0, 0, 120));
		m_highlightedRectangle.setFillColor(sf::Color(250, 250, 250, 100));

		m_text.setFont(m_font);
		m_caret.setFont(m_font);
		m_caret.setFillColor(sf::Color(230, 230, 230));
		m_caret.setString("I");

		setWidth(_width);
		setCharSize(m_charSize);

		selectAll();
		moveRight();
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setPosition(float _x, float _y) {
		setPosition({_x, _y});
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setPosition(const sf::Vector2f& _position) {
		m_rectangle.setPosition(_position);

		sf::Vector2f textPos{_position.x + m_xOffset,
		                     _position.y};

		setTextPosition(textPos);
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setBackgroundColor(const sf::Color& _color) {
		m_rectangle.setFillColor(_color);
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setCaretColor(const sf::Color& _color) {
		m_caret.setFillColor(_color);
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setFillColor(const sf::Color& _c) {
		m_fillColor = _c;
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setOutlineColor(const sf::Color& _c) {
		m_outlineColor = _c;
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setOutlineThickness(float _f) {
		m_outlineThickness = _f;
	}

	////////////////////////////////////////////////////////////
	const sf::Color TextEntryBox::getBackgroundColor() const {
		return m_rectangle.getFillColor();
	}

	////////////////////////////////////////////////////////////
	const sf::Color TextEntryBox::getCaretColor() const {
		return m_caret.getFillColor();
	}

	////////////////////////////////////////////////////////////
	const sf::Color TextEntryBox::getFillColor() const {
		return m_fillColor;
	}

	////////////////////////////////////////////////////////////
	const sf::Color TextEntryBox::getOutlineColor() const {
		return m_outlineColor;
	}

	////////////////////////////////////////////////////////////
	float TextEntryBox::getOutlineThickness() const {
		return m_outlineThickness;
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setCharSize(unsigned int _i) {
		m_charSize = _i;
		m_text.setCharacterSize(m_charSize);
		m_caret.setCharacterSize(m_charSize);
		setWidth(getWidth());
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setMaxChars(unsigned int _i) {
		m_maxChars = _i;
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setAlwaysVisible(bool _b) {
		m_alwaysVisible = _b;
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setAlwaysActive(bool _b) {
		m_alwaysActive = _b;
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setWidth(float _width) {
		float oldWidth{m_rectangle.getSize().x};
		m_rectangle.setSize({_width, float(1.25 * getLineSpacing())});

		//If our new size is smaller, we'll shift the text to the left
		//if needed and update transparency if so. We'll also update
		//the transparency if the text is wider than the box.
		if (_width < oldWidth) {
			m_alphaUpdateNeeded = shiftTextToLeft() || textTooWide();
		}

		//Similar logic applies if the new width is greater
		else if (_width > oldWidth) {
			m_alphaUpdateNeeded = shiftTextToRight() || textTooWide();
		}
	}

	////////////////////////////////////////////////////////////
	float TextEntryBox::getWidth() const {
		return m_rectangle.getSize().x;
	}

	////////////////////////////////////////////////////////////
	float TextEntryBox::getHeight() const {
		return m_rectangle.getSize().y;
	}

	////////////////////////////////////////////////////////////
	float TextEntryBox::getLineSpacing() const {
		return m_font.getLineSpacing(m_charSize);
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::getInput(sf::Event& _event) {
		switch (_event.type) {
		case sf::Event::KeyPressed: {
			if (_event.key.code == TEXT_RETURN) {
				if (m_enteringText && !stringEmpty()) {
					m_inputComplete = true;
					m_lastString    = m_text.getString();
					if (!m_alwaysActive) {
						clearText();
					}
				}
			}

			else if (keysPressedTogether({LSHIFT, TEXT_SELECTLEFT})) {
				selectLeft();
			}

			else if (keysPressedTogether({LSHIFT, TEXT_SELECTRIGHT})) {
				selectRight();
			}

			else if (keysPressedTogether({LCTRL, TEXT_SELECTALL})) {
				selectAll();
			}

			else if (keysPressedTogether({LCTRL, TEXT_COPY})) {
				sf::Clipboard::setString(getSelection());
			}

			else if (keysPressedTogether({LCTRL, TEXT_CUT})) {
				sf::Clipboard::setString(getSelection());
				if (sequenceSelected()) {
					deleteSelection();
				}
			}

			else if (keysPressedTogether({LCTRL, TEXT_PASTE})) {
				if (sequenceSelected()) {
					deleteSelection();
				}
				std::string s{sf::Clipboard::getString()};
				insert(s);
			}

			else if (keyPressed(TEXT_MOVELEFT)) {
				moveLeft();
			}

			else if (keyPressed(TEXT_MOVERIGHT)) {
				moveRight();
			}

			break;
		}

		case sf::Event::TextEntered: {
			if (!m_enteringText) {
				break;
			}
			handleInput(_event.text.unicode);
			break;
		}

		default:
			break;
		}
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::update() {
		if (m_alwaysActive) {
			m_enteringText = true;
		}
		if (m_enteringText) {
			updateHighlight();
			updateCaret();
		}
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::draw(sf::RenderTarget& target,
	                        sf::RenderStates  states) const {
		updateAlpha();

		if (m_enteringText || m_alwaysVisible) {
			target.draw(m_rectangle, states);
			target.draw(m_text, states);
			target.draw(m_highlightedRectangle, states);
		}
		if (m_enteringText && !sequenceSelected()) {
			target.draw(m_caret, states);
		}
	}

	////////////////////////////////////////////////////////////
	bool TextEntryBox::enteringText() const {
		return m_enteringText;
	}

	////////////////////////////////////////////////////////////
	bool TextEntryBox::inputComplete() const {
		if (m_inputComplete) {
			m_inputComplete = false;
			return true;
		}
		return false;
	}

	////////////////////////////////////////////////////////////
	const std::string TextEntryBox::getLastString() const {
		return m_lastString;
	}

	////////////////////////////////////////////////////////////
	const std::string TextEntryBox::getCurrentString() const {
		return m_text.getString();
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setActive(bool _b) {
		m_enteringText = _b;
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::handleInput(sf::Uint32 _unicode) {
		//Since typing "A" counts as text entry, we'll see if
		//CTRL is held too and not treat it as such if that's the
		//case. If it isn't, we'll delete the selection.
		if (sequenceSelected() && !keyPressed(LCTRL)) {
			deleteSelection();
		}

		//8 = backspace
		//for some reason, 127 (delete)
		//isn't registering
		if (_unicode == 8) {
			deleteSelection();
		}

		else if (validInsertion(_unicode)) {
			if (sequenceSelected()) {
				deleteSelection();
			}

			insert(static_cast<char>(_unicode));
		}
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setTextPosition(const sf::Vector2f& _pos) {
		m_text.setPosition(_pos);

		updateCaret();
		updateHighlight();
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::setTextString(const std::string& _str) {
		m_text.setString(_str);
		m_text.setOutlineThickness(m_outlineThickness);
		updateCaret();
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::updateAlpha() const {
		if (!m_alphaUpdateNeeded) {
			return;
		}

		m_alphaUpdateNeeded = false;

		auto        rectPos{m_rectangle.getPosition()};
		auto        rectBounds{m_rectangle.getGlobalBounds()};
		std::string textStr{m_text.getString()};

		for (size_t i{0}; i < textStr.length(); i++) {
			auto pos{m_text.findCharacterPos(i)};
			char charAtPos{textStr.at(i)};
			auto glyph{m_font.getGlyph(charAtPos, m_charSize, false, m_text.getOutlineThickness(i))};
			auto charWidth{glyph.bounds.width};
			auto fillColor{m_fillColor};
			auto outlineColor{m_outlineColor};

			if (pos.x < rectPos.x || (pos.x + charWidth) > (rectPos.x + rectBounds.width)) {
				fillColor    = sf::Color::Transparent;
				outlineColor = sf::Color::Transparent;
			}

			m_text.setFillColor(fillColor, i, i);
			m_text.setOutlineColor(outlineColor, i, i);
		}
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::updateCaret() {
		auto caretPos{m_rectangle.getPosition()};

		if (!stringEmpty()) {
			caretPos = m_text.findCharacterPos(m_selectionBegin);
			if (m_selectionBegin > 0) {
				//We'll position the caret as accurately as possible,
				//by positioning it exactly between the current and
				//previous characters
				size_t      prevPos{m_selectionBegin - 1};
				std::string textStr{m_text.getString()};
				char        charAtPrevPos{textStr.at(prevPos)};
				auto        prevGlyph{m_font.getGlyph(charAtPrevPos, m_charSize, false, m_text.getOutlineThickness(prevPos))};
				auto        prevCharPos{m_text.findCharacterPos(prevPos)};
				auto        prevCharWidth{prevGlyph.bounds.width};
				prevCharPos.x += prevCharWidth;
				caretPos.x = prevCharPos.x + ((caretPos.x - prevCharPos.x) / 2);
			}
		}

		m_caret.setPosition(caretPos);

		if (m_enteringText) {
			static bool      caretAlphaDecreasing{true};
			static sf::Uint8 caretAlphaValue{255};

			if (caretAlphaDecreasing) {
				caretAlphaValue -= 5;
				if (caretAlphaValue <= 0) {
					caretAlphaDecreasing = false;
				}
			}
			else {
				caretAlphaValue += 5;
				if (caretAlphaValue >= 255) {
					caretAlphaDecreasing = true;
				}
			}

			sf::Color caretColor = m_caret.getFillColor();
			caretColor.a         = caretAlphaValue;
			m_caret.setFillColor(caretColor);
		}
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::updateHighlight() {
		if (sequenceSelected()) {
			auto startPos{m_text.findCharacterPos(m_selectionBegin)};
			auto endPos{m_text.findCharacterPos(m_selectionEnd)};

			float maxStartX{m_rectangle.getPosition().x};
			float maxEndX{m_rectangle.getPosition().x + m_rectangle.getLocalBounds().width};

			if (startPos.x < maxStartX) {
				startPos.x = maxStartX;
			}
			if (endPos.x > maxEndX) {
				endPos.x = maxEndX;
			}

			m_highlightedRectangle.setSize(
			  {(endPos.x - startPos.x), m_rectangle.getLocalBounds().height});

			m_highlightedRectangle.setPosition(startPos.x, m_text.getPosition().y);
		}
		else {
			m_highlightedRectangle.setSize({0, 0});
		}
	}

	////////////////////////////////////////////////////////////
	bool TextEntryBox::stringEmpty() const {
		return m_text.getString().isEmpty();
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::selectAll() {
		m_selectionBegin     = 0;
		m_selectionEnd       = m_text.getString().getSize();
		m_selectionDirection = SELDIR::NEUTRAL;
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::unselectAll() {
		m_selectionEnd       = m_selectionBegin;
		m_selectionDirection = SELDIR::NEUTRAL;
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::selectLeft() {
		if (m_selectionDirection == SELDIR::NEUTRAL || m_selectionDirection == SELDIR::LEFT) {
			if (keyPressed(LCTRL)) {
				m_selectionBegin = posAtPreviousWord(m_selectionBegin);
			}
			else {
				moveSelectionBegin(-1);
			}

			m_selectionDirection = SELDIR::LEFT;
		}
		else {
			if (keyPressed(LCTRL)) {
				m_selectionEnd = posAtPreviousWord(m_selectionEnd);
			}
			else {
				moveSelectionEnd(-1);
			}

			if (m_selectionEnd < m_selectionBegin) {
				swapBeginAndEnd();
				m_selectionDirection = SELDIR::LEFT;
			}
		}

		m_alphaUpdateNeeded = shiftTextToRight();
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::selectRight() {
		if (m_selectionDirection == SELDIR::NEUTRAL || m_selectionDirection == SELDIR::RIGHT) {
			if (keyPressed(LCTRL)) {
				m_selectionEnd = posAtNextWord(m_selectionEnd);
			}
			else {
				moveSelectionEnd(1);
			}

			m_selectionDirection = SELDIR::RIGHT;
		}
		else {
			if (keyPressed(LCTRL)) {
				m_selectionBegin = posAtNextWord(m_selectionBegin);
			}
			else {
				moveSelectionBegin(1);
			}
			if (m_selectionEnd < m_selectionBegin) {
				swapBeginAndEnd();
				m_selectionDirection = SELDIR::RIGHT;
			}
		}

		m_alphaUpdateNeeded = shiftTextToLeft();
	}

	////////////////////////////////////////////////////////////
	bool TextEntryBox::shiftTextToLeft() {
		auto caretPos{m_caret.getPosition()};
		auto caretBounds{m_caret.getGlobalBounds()};
		auto boxPos{m_rectangle.getPosition()};
		auto boxBounds{m_rectangle.getGlobalBounds()};

		float extraSpace{(caretPos.x + caretBounds.width) -
		                 (boxPos.x + boxBounds.width)};

		if (extraSpace > -1 * caretBounds.width) {
			auto newTextPos{m_text.getPosition()};

			newTextPos.x -= extraSpace;
			newTextPos.x -= 2 * m_caret.getGlobalBounds().width;

			m_xOffset -= extraSpace;
			m_xOffset -= 2 * m_caret.getGlobalBounds().width;

			setTextPosition(newTextPos);

			return true;
		}

		return false;
	}

	////////////////////////////////////////////////////////////
	bool TextEntryBox::shiftTextToRight() {
		auto caretPos{m_caret.getPosition()};
		auto caretBounds{m_caret.getGlobalBounds()};
		auto boxPos{m_rectangle.getPosition()};
		auto boxBounds{m_rectangle.getGlobalBounds()};
		auto textPos{m_text.getPosition()};

		//If the caret is off to the left of the box, or if the text can
		//fit within the box and we've applied a negative offset, we'll
		//reset the text's position
		if (caretPos.x <= textPos.x || (m_xOffset < 0 && !textTooWide())) {
			resetTextPosition();
			return true;
		}

		float extraSpace{(caretPos.x + caretBounds.width) -
		                 (boxPos.x)};

		if (extraSpace < caretBounds.width) {
			auto newTextPos{m_text.getPosition()};

			newTextPos.x -= extraSpace;
			newTextPos.x += 2 * m_caret.getGlobalBounds().width;

			m_xOffset -= extraSpace;
			m_xOffset += 2 * m_caret.getGlobalBounds().width;

			setTextPosition(newTextPos);

			return true;
		}

		return false;
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::resetTextPosition() {
		auto textPos{m_text.getPosition()};
		textPos.x = m_rectangle.getPosition().x;
		m_xOffset = 0.F;
		setTextPosition(textPos);

		m_alphaUpdateNeeded = shiftTextToLeft();
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::moveLeft() {
		if (keyPressed(LCTRL)) {
			m_selectionBegin = posAtPreviousWord(m_selectionBegin);
		}
		else if (!sequenceSelected() && m_selectionBegin > 0) {
			moveSelectionBegin(-1);
		}

		m_selectionEnd       = m_selectionBegin;
		m_selectionDirection = SELDIR::NEUTRAL;

		updateCaret();
		m_alphaUpdateNeeded = (shiftTextToRight() || textTooWide());
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::moveRight() {
		//In order to prevent skipping to the next word when
		//pasting text (since the paste shortcut is CTRL+V),
		//we'll check that we aren't pasting.
		if (keyPressed(LCTRL) && !keyPressed(TEXT_PASTE)) {
			m_selectionBegin = posAtNextWord(m_selectionBegin);
			m_selectionEnd   = m_selectionBegin;
		}
		else if (sequenceSelected()) {
			m_selectionBegin = m_selectionEnd;
		}
		else if (m_selectionBegin < m_text.getString().getSize()) {
			moveSelectionBegin(1);
			m_selectionEnd = m_selectionBegin;
		}

		m_selectionDirection = SELDIR::NEUTRAL;

		updateCaret();

		m_alphaUpdateNeeded = (shiftTextToLeft() || textTooWide());
	}

	////////////////////////////////////////////////////////////
	size_t TextEntryBox::posAtPreviousWord(size_t _from) const {
		size_t pos{_from};
		if (pos == 0) {
			return pos;
		}

		std::string s{m_text.getString()};
		std::string previousChar{s.substr(pos - 1, 1)};
		if (previousChar == " ") {
			while (pos > 0 && s.substr(pos - 1, 1) == " ") {
				--pos;
			}
		}

		while (pos > 0 && s.substr(pos - 1, 1) != " ") {
			--pos;
		}

		return pos;
	}

	////////////////////////////////////////////////////////////
	size_t TextEntryBox::posAtNextWord(size_t _from) const {
		size_t      pos{_from};
		std::string s{m_text.getString()};
		if (pos == s.length()) {
			return pos;
		}

		std::string previousChar{s.substr(pos, 1)};
		if (previousChar == " ") {
			while (pos < s.length() && s.substr(pos, 1) == " ") {
				++pos;
			}
		}

		while (pos < s.length() && s.substr(pos, 1) != " ") {
			++pos;
		}

		return pos;
	}

	////////////////////////////////////////////////////////////
	bool TextEntryBox::sequenceSelected() const {
		return (m_selectionBegin != m_selectionEnd);
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::deleteSelection() {
		if (stringEmpty()) {
			return;
		}

		std::string newString{m_text.getString()};

		if (sequenceSelected()) {
			newString.erase(m_selectionBegin, m_selectionEnd - m_selectionBegin);
		}
		else {
			selectLeft();
			newString.erase(m_selectionBegin, 1);
		}

		m_selectionEnd       = m_selectionBegin;
		m_selectionDirection = SELDIR::NEUTRAL;
		setTextString(newString);

		updateCaret();
		shiftTextToRight();

		m_alphaUpdateNeeded = (textTooWide());
	}

	////////////////////////////////////////////////////////////
	const std::string TextEntryBox::getSelection() const {
		std::string s{m_text.getString()};
		return s.substr(m_selectionBegin, m_selectionEnd);
	}

	////////////////////////////////////////////////////////////
	bool TextEntryBox::validInsertion(sf::Uint32 _unicode) const {
		if (_unicode >= 32 && _unicode <= 255 &&
		    m_text.getString().getSize() < m_maxChars) {
			return true;
		}

		return false;
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::insert(std::string& _str) {
		_str.erase(std::remove_if(_str.begin(),
		                          _str.end(),
		                          [](char c) {
			                          return (c == '\n' || c == '\t' || c == '\r');
		                          }),
		           _str.end());

		if (_str.empty()) {
			return;
		}

		size_t currentLength{m_text.getString().getSize()};
		size_t combinedLength{_str.length() + currentLength};

		if (combinedLength > m_maxChars) {
			_str = _str.substr(0, m_maxChars - currentLength);
		}

		std::string newString{m_text.getString()};
		newString.insert(m_selectionBegin, _str);

		setTextString(newString);
		//In some cases, like after selecting all and inputting
		//a character, our caret will be too far off to the left.
		//
		//shiftTextToRight() will take care of that if so, and
		//do nothing if not.
		shiftTextToRight();

		m_selectionEnd       = m_selectionBegin + _str.length() - 1;
		m_selectionBegin     = m_selectionEnd;
		m_selectionDirection = SELDIR::NEUTRAL;

		moveRight();
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::insert(char _char) {
		size_t newLength{m_text.getString().getSize() + 1};

		if (newLength > m_maxChars) {
			return;
		}

		std::string newString{m_text.getString()};
		newString.insert(m_selectionBegin, {_char});

		setTextString(newString);
		//In some cases, like after selecting all and inputting
		//a character, our caret will be too far off to the left.
		//
		//shiftTextToRight() will take care of that if so, and
		//do nothing if not.
		shiftTextToRight();

		m_selectionEnd       = m_selectionBegin;
		m_selectionDirection = SELDIR::NEUTRAL;

		moveRight();
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::clearText() {
		m_selectionBegin     = 0;
		m_selectionEnd       = 0;
		m_selectionDirection = SELDIR::NEUTRAL;
		setTextString("");
		resetTextPosition();
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::moveSelectionBegin(int _a) {
		int d{int(m_selectionBegin) + _a};
		if (d < 0) {
			m_selectionBegin = 0;
			return;
		}
		else if (d > m_text.getString().getSize()) {
			m_selectionBegin = m_text.getString().getSize();
			return;
		}
		m_selectionBegin += _a;
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::moveSelectionEnd(int _a) {
		int d{int(m_selectionEnd) + _a};
		if (d < 0) {
			m_selectionEnd = 0;
			return;
		}
		else if (d > m_text.getString().getSize()) {
			m_selectionEnd = m_text.getString().getSize();
			return;
		}
		m_selectionEnd += _a;
	}

	////////////////////////////////////////////////////////////
	void TextEntryBox::swapBeginAndEnd() {
		size_t t{m_selectionEnd};
		m_selectionEnd   = m_selectionBegin;
		m_selectionBegin = t;
	}

	////////////////////////////////////////////////////////////
	bool TextEntryBox::textTooWide() const {
		auto textWidth{m_text.getGlobalBounds().width};
		auto boxWidth{m_rectangle.getGlobalBounds().width};

		return textWidth >= boxWidth;
	}

} //namespace spss
