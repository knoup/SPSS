#include <SPSS/Graphics/TextEntryPrompt.h>

#include <SPSS/Util/Input.h>

constexpr float MIN_WIDTH {200.F};
constexpr float MIN_HEIGHT{100.F};

namespace spss {
	TextEntryPrompt::TextEntryPrompt(const sf::Vector2f& _size,
	                                 const sf::Vector2f& _position,
	                                 const sf::Font&     _font,
	                                 const std::string&  _promptTitle,
	                                 const unsigned int  _charSize,
	                                 const std::string&  _defaultStr)
	            : m_window{nullptr},
	              m_lastMousePosition{},
	              m_dragging{false},
				  m_rect{{_size.x >= MIN_WIDTH ? _size.x : MIN_WIDTH,
	                         _size.y >= MIN_HEIGHT ? _size.y : MIN_HEIGHT}},
	              m_textEntry{_size.x >= MIN_WIDTH ? _size.x : MIN_WIDTH,
	                          _position,
	                          _font,
	                          _charSize,
	                          _defaultStr},
				  m_lastPosition{_position},
				  m_alignmentNeeded{true} {
		m_rect.setFillColor({0, 0, 0, 100});
		setPosition(_position);
		//m_infoBox.appendMessage({_promptTitle,""})
		//m_infoBox.setDraggable(true);
		m_textEntry.setAlwaysActive(true);

		m_confirmButton.setSize({100, 30});
		m_cancelButton.setSize({100, 30});

		m_confirmButton.setOrigin(m_confirmButton.getGlobalBounds().width / 2,
		                          m_confirmButton.getGlobalBounds().height / 2);

		m_cancelButton.setOrigin(m_cancelButton.getGlobalBounds().width / 2,
		                         m_cancelButton.getGlobalBounds().height / 2);

		m_confirmButton.setFillColor(sf::Color::Green);
		m_cancelButton.setFillColor(sf::Color::Red);

		alignElements();
	}

	void TextEntryPrompt::getInput(sf::Event& _e) {
		m_textEntry.getInput(_e);

		if(m_window == nullptr) {
			return;
		}

		if (Util::Input::lmbPressed(_e)) {
			auto view{m_window->getView()};
			auto mousePos = sf::Mouse::getPosition(*m_window);
			auto pixelPos{m_window->mapPixelToCoords(mousePos, view)};
			auto bounds{m_rect.getGlobalBounds()};

			if (bounds.contains(pixelPos.x, pixelPos.y)) {
				m_lastMousePosition = mousePos;

				if (bounds.contains(pixelPos.x, pixelPos.y)) {
					m_dragging = true;
				}
			}
		}

		else if (Util::Input::lmbReleased(_e)) {
			m_dragging = false;
		}
	}

	void TextEntryPrompt::update() {
		dragBox();
		m_textEntry.update();

		if (m_rect.getPosition() != m_lastPosition) {
			m_lastPosition = m_rect.getPosition();
			m_alignmentNeeded = true;
		}

		alignElements();
	}

	void TextEntryPrompt::draw(sf::RenderWindow& window, sf::RenderStates states) const {
		m_window = &window;
		window.draw(m_rect, states);
		window.draw(m_textEntry, states);
		window.draw(m_confirmButton, states);
		window.draw(m_cancelButton, states);
	}

	const std::string& TextEntryPrompt::getString() const {
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

	void TextEntryPrompt::setPosition(const sf::Vector2f& _pos) {
		m_rect.setPosition(_pos);
		m_alignmentNeeded = true;
	}

	void TextEntryPrompt::setSize(const sf::Vector2f& _size) {
		float w{_size.x >= MIN_WIDTH ? _size.x : MIN_WIDTH};
		float h{_size.y >= MIN_HEIGHT ? _size.y : MIN_HEIGHT};

		m_rect.setSize({w, h});
		m_textEntry.setWidth(w);
		m_alignmentNeeded = true;
	}

	void TextEntryPrompt::setOrigin(const sf::Vector2f& _origin) {
		m_rect.setOrigin(_origin);
		m_alignmentNeeded = true;
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

		if(newPos.x < 0){newPos.x = 0;}
		if(newPos.x + bounds.width > m_window->getSize().x){newPos.x = m_window->getSize().x - bounds.width;}
		if(newPos.y < 0){newPos.y = 0;}
		if(newPos.y + bounds.height > m_window->getSize().y){newPos.y = m_window->getSize().y - bounds.height;}

		setPosition(newPos);
	}

	void TextEntryPrompt::alignElements() {
		if (!m_alignmentNeeded) {
			return;
		}

		m_alignmentNeeded = false;

		auto pos{m_rect.getPosition()};
		auto size{m_rect.getSize()};

		m_textEntry.setPosition(pos);

		m_confirmButton.setPosition({pos.x + (size.x * 0.25F), pos.y + (size.y * 0.75F)});
		m_cancelButton.setPosition({pos.x + (size.x * 0.75F), pos.y + (size.y * 0.75F)});
	}

} // namespace spss
