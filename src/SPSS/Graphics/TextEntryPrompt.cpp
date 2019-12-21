#include <SPSS/Graphics/TextEntryPrompt.h>

#include <SPSS/Util/Input.h>

#include <iostream>

/// Since we're going to have an outline thickness of 1, we'll subtract
/// 1 from the given width/height in the constructor.

constexpr float BUTTON_HEIGHT{30.F};
constexpr float BUTTON_SPACING{5.F};

constexpr float MAX_WIDTH{550.F};
constexpr float MIN_WIDTH{300.F};

const sf::Color HIGHLIGHTED_BORDER{sf::Color::White};
const sf::Color UNHIGHLIGHTED_BORDER{255, 255, 255, 75};

namespace spss {
	TextEntryPrompt::TextEntryPrompt(const sf::Vector2f&      _position,
	                                 const sf::Font&          _font,
	                                 const std::string&       _promptTitle,
	                                 const unsigned int       _charSize,
	                                 const std::string&       _defaultStr)
	            : m_window{nullptr},
	              m_font{_font},
	              m_lastMousePosition{},
	              m_dragging{false},
	              m_rect{{MAX_WIDTH, 100}},
	              m_textEntry{300,
	                          _position,
	                          m_font,
	                          _charSize,
	                          _defaultStr},
	              m_lastPosition{_position},
	              m_alignmentNeeded{true} {
		m_title.setCharacterSize(22);
		m_title.setOutlineThickness(1);
		m_title.setOutlineColor(sf::Color::Black);
		m_title.setFont(m_font);
		m_title.setString(_promptTitle);

		m_rect.setFillColor({0, 0, 0, 100});
		m_rect.setOutlineThickness(1);

		float titleWidth{m_title.getGlobalBounds().width};
		float rectWidth{m_rect.getGlobalBounds().width};

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
		m_textEntry.setAlwaysActive(true);

		alignElements();
	}

	void TextEntryPrompt::getInput(sf::Event& _e) {
		m_textEntry.getInput(_e);

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

	void TextEntryPrompt::update() {
		dragBox();
		m_textEntry.update();

		if (m_rect.getPosition() != m_lastPosition) {
			m_lastPosition    = m_rect.getPosition();
			m_alignmentNeeded = true;
		}

		alignElements();
	}

	void TextEntryPrompt::draw(sf::RenderWindow& window, sf::RenderStates states) const {
		m_window = &window;
		window.draw(m_rect, states);
		window.draw(m_title, states);
		window.draw(m_textEntry, states);
		for (auto& b : m_buttons) {
			window.draw(b.m_shape);
			window.draw(b.m_text);
		}
	}

	void TextEntryPrompt::addButton(const std::string& _str, spss::Function<std::any> _action) {
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

	const std::string TextEntryPrompt::getString() const {
		return m_textEntry.getCurrentString();
	}

	const sf::Vector2f& TextEntryPrompt::getPosition() const {
		return m_rect.getPosition();
	}

	const sf::Vector2f& TextEntryPrompt::getSize() const {
		return m_rect.getSize();
	}

	const sf::FloatRect& TextEntryPrompt::getLocalBounds() const {
		return m_rect.getLocalBounds();
	}

	const sf::FloatRect& TextEntryPrompt::getGlobalBounds() const {
		return m_rect.getGlobalBounds();
	}

	const sf::Color& TextEntryPrompt::getColor() const {
		return m_rect.getFillColor();
	}

	void TextEntryPrompt::setPosition(const sf::Vector2f& _pos) {
		m_rect.setPosition(_pos);
		m_alignmentNeeded = true;
	}

	void TextEntryPrompt::setOrigin(const sf::Vector2f& _origin) {
		m_rect.setOrigin(_origin);
		m_alignmentNeeded = true;
	}

	void TextEntryPrompt::setColor(const sf::Color& _color) {
		m_rect.setFillColor(_color);
		m_textEntry.setBackgroundColor(_color);
	}

	////////////////////////////////////////////////////////////
	void TextEntryPrompt::fitWidth(float _width) {
		std::string str{m_title.getString()};
		size_t lines = std::count(str.begin(), str.end(), '\n');

		bool tooWide{m_title.getGlobalBounds().width > _width};
		bool tooNarrow{m_title.getGlobalBounds().width < _width &&
		               lines > 1};

		if (!tooWide && !tooNarrow) {
			return;
		}

		str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());

		float lineWidth{0};

		for (size_t i{0}; i < str.length(); i++) {
			auto pos{m_title.findCharacterPos(i)};
			char charAtPos{str.at(i)};
			auto glyph{m_title.getFont()->getGlyph(charAtPos, m_title.getCharacterSize(), false, m_title.getOutlineThickness())};

			float charWidth{glyph.bounds.width};
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

	void TextEntryPrompt::setHeight(float _height) {
		m_rect.setSize({m_rect.getSize().x, _height});
		m_alignmentNeeded = true;
	}

	void TextEntryPrompt::setWidth(float _width) {
		m_rect.setSize({_width, m_rect.getSize().y});
		m_textEntry.setWidth(_width);
		fitWidth(_width);
		m_alignmentNeeded = true;
	}

	void TextEntryPrompt::handleMouseClick() {
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

	void TextEntryPrompt::handleMouseover() {
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

	void TextEntryPrompt::dragBox() {
		if (!m_dragging || m_window == nullptr) {
			return;
		}
		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);

		sf::Vector2i diff{mousePos.x - m_lastMousePosition.x,
		                  mousePos.y - m_lastMousePosition.y};

		if (m_lastMousePosition == mousePos) {
			return;
		}

		m_lastMousePosition = mousePos;

		sf::Vector2f newPos{m_rect.getPosition()};
		newPos.x += diff.x;
		newPos.y += diff.y;

		auto bounds{m_rect.getGlobalBounds()};

		if (newPos.x < 0) {
			newPos.x = 0;
		}
		if (newPos.x + bounds.width > m_window->getSize().x) {
			newPos.x = m_window->getSize().x - bounds.width;
		}
		if (newPos.y < 0) {
			newPos.y = 0;
		}
		if (newPos.y + bounds.height > m_window->getSize().y) {
			newPos.y = m_window->getSize().y - bounds.height;
		}

		setPosition(newPos);
	}

	void TextEntryPrompt::alignElements() {
		if (!m_alignmentNeeded) {
			return;
		}

		m_alignmentNeeded = false;

		auto pos{m_rect.getPosition()};
		auto size{m_rect.getSize()};

		m_title.setPosition(pos);

		auto titleBounds{m_title.getGlobalBounds()};

		m_textEntry.setPosition({pos.x, 10.F + (titleBounds.top + (titleBounds.height))});

		//We'll get the FloatRect describing the space where the buttons
		//can be placed - that is, everything below the text entry box

		auto rectBounds{m_rect.getGlobalBounds()};
		auto textEntryBounds{m_textEntry.getGlobalBounds()};

		float rectBottom{rectBounds.top + rectBounds.height};
		float textEntryBottom{textEntryBounds.top + textEntryBounds.height};

		sf::FloatRect buttonBounds{rectBounds.left,
		                           textEntryBottom + BUTTON_SPACING,
		                           size.x,
		                           rectBottom - textEntryBottom};

		//We'll make sure that the remainder of the box fits
		//the buttons just right - it will expand if it's too
		//small, or shrink if it's too large

		float heightNeeded{(m_buttons.size() * BUTTON_HEIGHT)
						   +
						   ((m_buttons.size() + 1) * BUTTON_SPACING)};

		float diff{buttonBounds.height - heightNeeded};

		if(diff != 0) {
			setHeight(size.y - diff);
			return;
		}

		float centerX{pos.x + size.x / 2};

		for (int i{0}; i < m_buttons.size(); i++) {
			float y{buttonBounds.top + (i * BUTTON_HEIGHT)};
			y += i * BUTTON_SPACING;
			m_buttons[i].setPosition({centerX, y});
		}
	}

} // namespace spss
