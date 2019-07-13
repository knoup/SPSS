#include <SPSS/Graphics/TextEntryBox.h>

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

const bool keysPressedTogether(std::vector<sf::Keyboard::Key> _keys) {
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

    TextEntryBox::TextEntryBox(const sf::Font&    _font,
                               float              _width,
                               const std::string& _str)
        : m_font{_font},
          m_rectangle(),
          m_selectionBegin(0),
          m_selectionEnd(0),
          m_selectionDirection(0),
          m_text(),
          m_enteringText{false},
          m_inputComplete{false},
          m_alwaysVisible{false},
          m_alwaysActive{false},
          m_lastString{},
          m_charSize{18},
          m_maxChars{255},
          m_fillColor{sf::Color::White},
          m_outlineColor{sf::Color::Black},
          m_outlineThickness{0} {

        setTextString(_str);

        m_rectangle.setFillColor(sf::Color(0, 0, 0, 120));
        m_highlightedRectangle.setFillColor(sf::Color(250, 250, 250, 100));

        m_text.setFont(m_font);
        m_caret.setFont(m_font);
        m_caret.setFillColor(sf::Color(230, 230, 230));
        m_caret.setString("|");

        setWidth(_width);
        setCharSize(m_charSize);

        selectAll();
        moveRight();
    }

    void TextEntryBox::setPosition(float _x, float _y) {
        setPosition({_x, _y});
    }

    void TextEntryBox::setPosition(const sf::Vector2f& _position) {
        m_rectangle.setPosition(_position);
        setTextPosition(_position);
    }

    void TextEntryBox::setBackgroundColor(const sf::Color& _color) {
        m_rectangle.setFillColor(_color);
    }

    void TextEntryBox::setCaretColor(const sf::Color& _color) {
        m_caret.setColor(_color);
    }

    void TextEntryBox::setFillColor(const sf::Color& _c) {
        m_fillColor = _c;
    }

    void TextEntryBox::setOutlineColor(const sf::Color& _c) {
        m_outlineColor = _c;
    }

    void TextEntryBox::setOutlineThickness(float _f) {
        m_outlineThickness = _f;
    }

    const sf::Color TextEntryBox::getBackgroundColor() const {
        return m_rectangle.getFillColor();
    }

    const sf::Color TextEntryBox::getCaretColor() const {
        return m_caret.getFillColor();
    }

    const sf::Color TextEntryBox::getFillColor() const {
        return m_fillColor;
    }

    const sf::Color TextEntryBox::getOutlineColor() const {
        return m_outlineColor;
    }

    float TextEntryBox::getOutlineThickness() const {
        return m_outlineThickness;
    }

    void TextEntryBox::setCharSize(unsigned int _i) {
        m_charSize = _i;
        m_text.setCharacterSize(m_charSize);
        m_caret.setCharacterSize(m_charSize);
        setWidth(getWidth());
    }

    void TextEntryBox::setMaxChars(unsigned int _i) {
        m_maxChars = _i;
    }

    void TextEntryBox::setAlwaysVisible(bool _b) {
        m_alwaysVisible = _b;
    }

    void TextEntryBox::setAlwaysActive(bool _b) {
        m_alwaysActive = _b;
    }

    void TextEntryBox::setWidth(float _width) {
        m_rectangle.setSize({_width, float(1.25 * getLineSpacing())});
    }

    float TextEntryBox::getWidth() const {
        return m_rectangle.getSize().x;
    }

    float TextEntryBox::getLineSpacing() const {
        return m_font.getLineSpacing(m_charSize);
    }

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

                //Since typing "A" counts as text entry, we'll see if
                //CTRL is held too and not treat it as such if that's the
                //case. If it isn't, we'll delete the selection.
                if (sequenceSelected() && !keyPressed(LCTRL)) {
                    deleteSelection();
                }

                //8 = backspace
                //for some reason, 127 (delete)
                //isn't registering
                if (_event.text.unicode == 8) {
                    deleteSelection();
                }

                else if (validInsertion(_event.text.unicode)) {
                    if (sequenceSelected()) {
                        deleteSelection();
                    }

                    insert(static_cast<char>(_event.text.unicode));
                }

                break;
            }

            case sf::Event::Resized: {
                updateTransparency();
            }

            default:
                break;
        }
    }

    void TextEntryBox::update() {
        if (m_alwaysActive) {
            m_enteringText = true;
        }
        if (m_enteringText) {
            updateHighlight();
            updateCaret();
        }
    }

    void TextEntryBox::draw(sf::RenderTarget& target,
                            sf::RenderStates  states) const {
        if (m_enteringText || m_alwaysVisible) {
            target.draw(m_rectangle, states);
            target.draw(m_text, states);
            target.draw(m_highlightedRectangle, states);
        }
        if (m_enteringText && !sequenceSelected()) {
            target.draw(m_caret, states);
        }
    }

    bool TextEntryBox::enteringText() const {
        return m_enteringText;
    }

    bool TextEntryBox::inputComplete() const {
        if (m_inputComplete) {
            m_inputComplete = false;
            return true;
        }
        return false;
    }

    const std::string TextEntryBox::getLastString() const {
        return m_lastString;
    }

    const std::string TextEntryBox::getCurrentString() const {
        return m_text.getString();
    }

    void TextEntryBox::setActive(bool _b) {
        m_enteringText = _b;
    }

    void TextEntryBox::setTextPosition(const sf::Vector2f& _pos) {
        m_text.setPosition(_pos);
        updateCaret();
        updateHighlight();
        updateTransparency();
    }

    void TextEntryBox::setTextString(const std::string& _str) {
        m_text.setString(_str);
        m_text.setOutlineThickness(m_outlineThickness);
        shiftTextToRight();
    }

    void TextEntryBox::updateTransparency() {
        auto rectPos{m_rectangle.getPosition()};
        auto rectBounds{m_rectangle.getGlobalBounds()};
        std::string textStr{m_text.getString()};

        for(size_t i{0}; i < textStr.length(); i++) {
            auto pos{m_text.findCharacterPos(i)};
            char charAtPos{textStr.at(i)};
            auto glyph{m_font.getGlyph(charAtPos, m_charSize, false, m_text.getOutlineThickness(i))};
            auto charWidth{glyph.bounds.width};
            auto color{m_fillColor};

            if(pos.x < rectPos.x || (pos.x + charWidth) > (rectPos.x + rectBounds.width)) {
                color = sf::Color::Transparent;
            }
            m_text.setFillColor(color, i, i);
        }
    }

    void TextEntryBox::updateCaret() {
        auto caretPos{m_rectangle.getPosition()};

        if(!stringEmpty()) {
            caretPos   = m_text.findCharacterPos(m_selectionBegin);
            caretPos.x -= m_text.getLetterSpacing() * 2;
        }

        m_caret.setPosition(caretPos);

        if (m_enteringText) {
            static bool caretAlphaDecreasing{true};
            static int  caretAlphaValue{255};

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
            m_highlightedRectangle.setSize({0,0});
        }
    }

    bool TextEntryBox::stringEmpty() const {
        return m_text.getString().isEmpty();
    }

    void TextEntryBox::selectAll() {
        m_selectionBegin     = 0;
        m_selectionEnd       = m_text.getString().getSize();
        m_selectionDirection = 0;
    }

    void TextEntryBox::unselectAll() {
        m_selectionEnd       = m_selectionBegin;
        m_selectionDirection = 0;
    }

    void TextEntryBox::selectLeft() {
        if (keyPressed(LCTRL)) {
            m_selectionBegin = posAtPreviousWord();
        }
        else if (m_selectionDirection <= 0) {
            if (m_selectionBegin <= 0) {
                return;
            }
            --m_selectionBegin;
        }
        else {
            --m_selectionEnd;
        }
        --m_selectionDirection;

        shiftTextToRight();
    }

    void TextEntryBox::selectRight() {
        if (keyPressed(LCTRL)) {
            m_selectionEnd = posAtNextWord();
        }
        else if (m_selectionDirection >= 0) {
            if (m_selectionEnd >= m_text.getString().getSize()) {
                return;
            }
            ++m_selectionEnd;
        }
        else {
            ++m_selectionBegin;
        }
        ++m_selectionDirection;

        shiftTextToLeft();
    }

    void TextEntryBox::shiftTextToLeft() {
        float extraSpace{(m_caret.getPosition().x + m_caret.getGlobalBounds().width)
                         -
                         (m_rectangle.getPosition().x + m_rectangle.getGlobalBounds().width)};

        if(extraSpace > -1 * m_caret.getGlobalBounds().width) {
            auto pos{m_text.getPosition()};
            pos.x -= extraSpace;
            pos.x -= 2 * m_caret.getGlobalBounds().width;
            setTextPosition(pos);
        }

        updateTransparency();
    }

    void TextEntryBox::shiftTextToRight() {
        if(m_caret.getPosition().x <= m_text.getPosition().x) {
            resetTextPosition();
            return;
        }

        float extraSpace{(m_caret.getPosition().x + m_caret.getGlobalBounds().width)
                         -
                         (m_rectangle.getPosition().x)};

        if(extraSpace < m_caret.getGlobalBounds().width) {
            auto pos{m_text.getPosition()};
            pos.x -= extraSpace;
            pos.x += 2 * m_caret.getGlobalBounds().width;
            setTextPosition(pos);
        }

        updateTransparency();
    }

    void TextEntryBox::resetTextPosition() {
        auto textPos{m_text.getPosition()};
        textPos.x = m_rectangle.getPosition().x;
        setTextPosition(textPos);
    }

    void TextEntryBox::moveLeft() {
        if (keyPressed(LCTRL)) {
            m_selectionBegin = posAtPreviousWord();
            m_selectionEnd   = m_selectionBegin;
        }
        else if (!sequenceSelected() && m_selectionBegin > 0) {
            --m_selectionBegin;
            m_selectionEnd = m_selectionBegin;
        }
        else if (sequenceSelected()) {
            m_selectionEnd = m_selectionBegin;
        }
        m_selectionDirection = 0;

        updateCaret();
        shiftTextToRight();
    }

    void TextEntryBox::moveRight() {
        if (keyPressed(LCTRL)) {
            m_selectionBegin = posAtNextWord();
            m_selectionEnd   = m_selectionBegin;
        }
        else if (!sequenceSelected() &&
                 m_selectionBegin < m_text.getString().getSize()) {
            ++m_selectionBegin;
            m_selectionEnd = m_selectionBegin;
        }
        else if (sequenceSelected()) {
            m_selectionBegin = m_selectionEnd;
        }
        m_selectionDirection = 0;

        updateCaret();
        shiftTextToLeft();
    }

    size_t TextEntryBox::posAtPreviousWord() const {
        size_t pos{m_selectionBegin};
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

    size_t TextEntryBox::posAtNextWord() const {
        size_t      pos{m_selectionEnd};
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

    bool TextEntryBox::sequenceSelected() const {
        return (m_selectionBegin != m_selectionEnd);
    }

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
        m_selectionDirection = 0;
        setTextString(newString);

        updateCaret();
        shiftTextToRight();
    }

    const std::string TextEntryBox::getSelection() const {
        std::string s{m_text.getString()};
        return s.substr(m_selectionBegin, m_selectionEnd);
    }

    bool TextEntryBox::validInsertion(sf::Uint32 _unicode) const {
        if (_unicode >= 32 && _unicode <= 255 &&
                m_text.getString().getSize() < m_maxChars) {
            return true;
        }

        return false;
    }

    void TextEntryBox::insert(std::string& _str) {
        size_t currentLength{m_text.getString().getSize()};
        size_t combinedLength{_str.length() + currentLength};
        if (combinedLength > m_maxChars) {
            _str = _str.substr(0, m_maxChars - currentLength);
        }

        std::string newString{m_text.getString()};
        newString.insert(m_selectionBegin, _str);

        setTextString(newString);

        m_selectionEnd       = m_selectionBegin + _str.length();
        m_selectionBegin     = m_selectionEnd;
        m_selectionDirection = 0;

        updateCaret();
        shiftTextToLeft();
    }

    void TextEntryBox::insert(char _char) {
        std::string newString{m_text.getString()};
        newString.insert(m_selectionBegin, 1, _char);

        if (newString.length() > m_maxChars) {
            newString = newString.substr(0, m_maxChars);
        }

        setTextString(newString);
        moveRight();
    }

    void TextEntryBox::clearText() {
        m_selectionBegin     = 0;
        m_selectionEnd       = 0;
        m_selectionDirection = 0;
        setTextString("");
        resetTextPosition();
    }

} //namespace spss
