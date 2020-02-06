#include <SPSS/Graphics/DialogPrompt.h>

#include <SPSS/Util/Input.h>

constexpr float MAX_WIDTH{550.F};
constexpr float MIN_WIDTH{300.F};

constexpr float BUTTON_HEIGHT{30.F};
constexpr float BUTTON_SPACING{5.F};
constexpr float BUTTON_TOP_PADDING{10.F};
constexpr float BUTTON_BOTTOM_PADDING{10.F};

constexpr float TITLE_TOP_PADDING{5.F};
constexpr float TEXT_SIDE_PADDING{20.F};
constexpr float TEXTENTRY_TOP_PADDING{5.F};

const sf::Color HIGHLIGHTED_BORDER{sf::Color::White};
const sf::Color UNHIGHLIGHTED_BORDER{255, 255, 255, 75};

namespace spss {

	////////////////////////////////////////////////////////////
	DialogPrompt::DialogPrompt(bool                _textEntryEnabled,
	                           const sf::Vector2f& _position,
	                           const sf::Font&     _font,
	                           const std::string&  _promptTitle,
	                           const unsigned int  _titleCharSize,
	                           const std::string&  _boxDefaultStr,
	                           const unsigned int  _boxCharSize)
	            : m_window{nullptr},
	              m_font{_font},
	              m_lastMousePosition{},
	              m_lastPosition{_position},
	              m_draggable{true},
	              m_dragging{false},
	              m_rect{{MAX_WIDTH, 100}},
	              m_textEntry{!_textEntryEnabled ? nullptr : std::make_unique<TextEntryBox>(MAX_WIDTH, _position, m_font, _boxCharSize, _boxDefaultStr)},
	              m_alignmentNeeded{true} {
		m_title.setCharacterSize(_titleCharSize);
		m_title.setOutlineThickness(1);
		m_title.setOutlineColor(sf::Color::Black);
		m_title.setFont(m_font);
		m_title.setString(_promptTitle);

		m_rect.setFillColor({0, 0, 0, 100});
		m_rect.setOutlineThickness(1);

		float titleWidth{m_title.getGlobalBounds().width};

		if (titleWidth <= MIN_WIDTH) {
			setWidth(MIN_WIDTH);
		}

		else if (titleWidth <= MAX_WIDTH) {
			setWidth(titleWidth);
		}

		else {
			setWidth(MAX_WIDTH);
		}

		setPosition(_position);

		if (m_textEntry != nullptr) {
			m_textEntry->setAlwaysActive(true);
			//We'll make the text entry box transparent so that it
			//takes on the color of the dialog prompt
			m_textEntry->setBackgroundColor(sf::Color::Transparent);
		}

		alignElements();
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::getInput(sf::Event& _e) {
		if (m_textEntry != nullptr) {
			m_textEntry->getInput(_e);
		}

		if (m_window == nullptr) {
			return;
		}

		if (_e.type == sf::Event::MouseMoved) {
			handleMouseover();
		}
		else if (Util::Input::lmbPressed(_e)) {
			handleMouseClick();
		}
		else if (Util::Input::lmbReleased(_e)) {
			m_dragging = false;
		}
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::update() {
		dragBox();
		if (m_textEntry != nullptr) {
			m_textEntry->update();
		}

		if (m_rect.getPosition() != m_lastPosition) {
			m_lastPosition    = m_rect.getPosition();
			m_alignmentNeeded = true;
		}

		alignElements();
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::draw(sf::RenderWindow& window, sf::RenderStates states) const {
		m_window = &window;
		window.draw(m_rect, states);
		window.draw(m_title, states);
		if (m_textEntry != nullptr) {
			window.draw(*m_textEntry, states);
		}
		for (auto& b : m_buttons) {
			window.draw(b.m_shape);
			window.draw(b.m_text);
		}
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::addButton(const std::string& _str, spss::Function<std::any> _action) {
		Button b;

		b.m_action = _action;

		b.m_text.setCharacterSize(17);
		b.m_text.setFont(m_font);
		b.m_text.setString(_str);

		b.m_shape.setSize({(0.9F * m_rect.getSize().x), BUTTON_HEIGHT});

		b.m_shape.setFillColor(sf::Color::Transparent);
		b.m_shape.setOutlineColor(UNHIGHLIGHTED_BORDER);
		b.m_shape.setOutlineThickness(-1);

		b.m_shape.setOrigin(b.m_shape.getGlobalBounds().left +
		                      b.m_shape.getGlobalBounds().width / 2,
		                    0);

		b.m_text.setOrigin(b.m_text.getGlobalBounds().left +
		                     b.m_text.getGlobalBounds().width / 2,
		                   0);

		m_buttons.push_back(b);
		m_alignmentNeeded = true;
	}

	////////////////////////////////////////////////////////////
	const std::string DialogPrompt::getString() const {
		if (m_textEntry != nullptr) {
			return m_textEntry->getCurrentString();
		}
		else {
			return std::string{""};
		}
	}

	////////////////////////////////////////////////////////////
	const sf::Vector2f& DialogPrompt::getPosition() const {
		return m_rect.getPosition();
	}

	////////////////////////////////////////////////////////////
	const sf::Vector2f& DialogPrompt::getSize() const {
		return m_rect.getSize();
	}

	////////////////////////////////////////////////////////////
	const sf::FloatRect& DialogPrompt::getLocalBounds() const {
		return m_rect.getLocalBounds();
	}

	////////////////////////////////////////////////////////////
	const sf::FloatRect& DialogPrompt::getGlobalBounds() const {
		return m_rect.getGlobalBounds();
	}

	////////////////////////////////////////////////////////////
	const sf::Color& DialogPrompt::getColor() const {
		return m_rect.getFillColor();
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::setPosition(const sf::Vector2f& _pos) {
		m_rect.setPosition(_pos);
		m_alignmentNeeded = true;
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::setColor(const sf::Color& _color) {
		m_rect.setFillColor(_color);
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::setMaxChars(unsigned int _i) {
		if (m_textEntry != nullptr) {
			m_textEntry->setMaxChars(_i);
		}
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::setDraggable(bool _d) {
		m_draggable = _d;
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::fitWidth(float _width) {
		std::string str{m_title.getString()};
		size_t      lines = std::count(str.begin(), str.end(), '\n');

		//We'll reduce the effective width a little bit, just for
		//some padding on the sides.

		_width -= TEXT_SIDE_PADDING;

		bool tooWide{m_title.getGlobalBounds().width > _width};
		bool tooNarrow{m_title.getGlobalBounds().width < _width &&
		               lines > 1};

		if (!tooWide && !tooNarrow) {
			return;
		}

		str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());

		float lineWidth{0};

		for (size_t i{0}; i < str.length(); i++) {
			auto  pos{m_title.findCharacterPos(i)};
			char  charAtPos{str.at(i)};
			auto  glyph{m_title.getFont()->getGlyph(charAtPos, m_title.getCharacterSize(), false, m_title.getOutlineThickness())};
			float spacing{glyph.advance};

			//Insert a newline at the prior position
			//if needed
			if (lineWidth + spacing >= _width) {
				str.insert(i, 1, '\n');
				lineWidth = 0;
				i++;
			}

			lineWidth += spacing;
		}

		m_title.setString(str);
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::setHeight(float _height) {
		m_rect.setSize({m_rect.getSize().x, _height});

		auto bounds{m_rect.getLocalBounds()};
		m_rect.setOrigin(bounds.left + (bounds.width / 2),
		                 bounds.top + (bounds.height / 2));

		m_alignmentNeeded = true;
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::setWidth(float _width) {
		if (m_textEntry != nullptr) {
			m_textEntry->setWidth(_width - (2 * TEXT_SIDE_PADDING));
		}

		m_rect.setSize({_width, m_rect.getSize().y});
		fitWidth(_width);

		auto bounds{m_rect.getLocalBounds()};
		m_rect.setOrigin(bounds.left + (bounds.width / 2),
		                 bounds.top + (bounds.height / 2));

		m_alignmentNeeded = true;
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::handleMouseClick() {
		auto view{m_window->getView()};
		auto mousePos = sf::Mouse::getPosition(*m_window);
		auto pixelPos{m_window->mapPixelToCoords(mousePos, view)};

		auto bounds{m_rect.getGlobalBounds()};
		bool insideBox{bounds.contains(pixelPos.x, pixelPos.y)};

		for (auto& b : m_buttons) {
			auto buttonBounds{b.m_shape.getGlobalBounds()};
			bool insideButton{buttonBounds.contains(pixelPos.x, pixelPos.y)};
			bool buttonNull{b.m_action == nullptr};

			if (insideButton && !buttonNull) {
				b.m_action(std::any());
				return;
			}
		}

		if (insideBox) {
			m_lastMousePosition = mousePos;
			m_dragging          = true;
		}
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::handleMouseover() {
		auto view{m_window->getView()};
		auto mousePos = sf::Mouse::getPosition(*m_window);
		auto pixelPos{m_window->mapPixelToCoords(mousePos, view)};

		for (auto& b : m_buttons) {
			auto buttonBounds{b.m_shape.getGlobalBounds()};
			bool insideButton{buttonBounds.contains(pixelPos.x, pixelPos.y)};
			bool buttonNull{b.m_action == nullptr};

			if (insideButton && !buttonNull) {
				b.m_shape.setOutlineColor(HIGHLIGHTED_BORDER);
			}
			else {
				b.m_shape.setOutlineColor(UNHIGHLIGHTED_BORDER);
			}
		}
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::dragBox() {
		if (!m_draggable || !m_dragging || m_window == nullptr) {
			return;
		}
		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);

		sf::Vector2i diff{mousePos.x - m_lastMousePosition.x,
		                  mousePos.y - m_lastMousePosition.y};

		if (m_lastMousePosition == mousePos) {
			return;
		}

		m_lastMousePosition = mousePos;

		auto bounds{m_rect.getGlobalBounds()};

		sf::Vector2f newPos{m_rect.getPosition()};
		newPos.x += diff.x;
		newPos.y += diff.y;

		//validBounds will represent the range of valid positions that our dialog
		//prompt can have. Since our origin will be centered, the min x/y values
		//will be half the width and half the height respectively. The max x/y
		//values will be the edges of the screen - the width/height.

		sf::FloatRect validBounds{bounds.width / 2,
		                          bounds.height / 2,
		                          m_window->getView().getSize().x - (bounds.width),
		                          m_window->getView().getSize().y - (bounds.height)};

		if (newPos.x < validBounds.left) {
			newPos.x = validBounds.left;
		}
		else if (newPos.x > validBounds.left + validBounds.width) {
			newPos.x = validBounds.left + validBounds.width;
		}

		if (newPos.y < validBounds.top) {
			newPos.y = validBounds.top;
		}
		else if (newPos.y > validBounds.top + validBounds.height) {
			newPos.y = validBounds.top + validBounds.height;
		}

		setPosition(newPos);
	}

	////////////////////////////////////////////////////////////
	void DialogPrompt::alignElements() {
		if (!m_alignmentNeeded) {
			return;
		}

		m_alignmentNeeded = false;

		auto rectBounds{m_rect.getGlobalBounds()};

		//For the purpose of aligning elements, we'll want to get the position of
		//the upperleft most corner of m_rect. This is why we don't call m_rect.getPosition()
		//(which would return the center of the box, since the origin is always centered)

		sf::Vector2f pos{rectBounds.left, rectBounds.top};
		auto         size{m_rect.getSize()};

		m_title.setPosition({pos.x + TEXT_SIDE_PADDING, pos.y + TITLE_TOP_PADDING});

		auto titleBounds{m_title.getGlobalBounds()};

		if (m_textEntry != nullptr) {
			m_textEntry->setPosition({pos.x + TEXT_SIDE_PADDING,
			                          titleBounds.top + titleBounds.height + TEXTENTRY_TOP_PADDING});
		}

		//We'll get the FloatRect describing the space where the buttons
		//can be placed - that is, everything below the text entry box

		//If we have a text entry box, our buttons will appear directly
		//below the title, so we'll use the title's bounds instead.

		//If we have a text entry box though, the buttons will appear
		//below it.

		auto bottomElementBounds{m_textEntry == nullptr ? m_title.getGlobalBounds() : m_textEntry->getGlobalBounds()};

		float rectBottom{rectBounds.top + rectBounds.height};
		float bottomElementBottom{bottomElementBounds.top + bottomElementBounds.height};

		sf::FloatRect buttonBounds{rectBounds.left,
		                           bottomElementBottom + BUTTON_TOP_PADDING,
		                           size.x,
		                           rectBottom - bottomElementBottom};

		//We'll make sure that the remainder of the box fits
		//the buttons just right - it will expand if it's too
		//small, or shrink if it's too large

		//We wouldn't want to perform 0-1 on a size_t now, would we?
		float totalSpacing{0.F};
		if (m_buttons.size() > 0) {
			totalSpacing = (m_buttons.size() - 1) * BUTTON_SPACING;
		}

		float heightNeeded{(m_buttons.size() * BUTTON_HEIGHT) +
		                   totalSpacing};

		heightNeeded += BUTTON_TOP_PADDING;
		heightNeeded += BUTTON_BOTTOM_PADDING;

		float diff{buttonBounds.height - heightNeeded};

		//If a resize is needed, we'll just call setHeight() and return.
		//setHeight() will set m_alignment to true, so this function will
		//be called again with the updated, correct size the next frame.
		if (diff != 0) {
			setHeight(size.y - diff);
			return;
		}

		float centerX{pos.x + size.x / 2};

		for (size_t i{0}; i < m_buttons.size(); i++) {
			float y{buttonBounds.top + (i * BUTTON_HEIGHT)};
			y += i * BUTTON_SPACING;
			m_buttons[i].setPosition({centerX, y});
		}
	}

} // namespace spss
