#include <SPSS/Graphics/TextEntryPrompt.h>

#include <SPSS/Util/Input.h>

/// Since we're going to have an outline thickness of 1, we'll subtract
/// 1 from the given width/height in the constructor.

constexpr float MIN_WIDTH{200.F};
constexpr float MIN_HEIGHT{150.F};

const sf::Color HIGHLIGHTED_BORDER{sf::Color::White};
const sf::Color UNHIGHLIGHTED_BORDER{255, 255, 255, 75};

namespace spss {
	TextEntryPrompt::TextEntryPrompt(const sf::Vector2f&      _size,
	                                 const sf::Vector2f&      _position,
	                                 const sf::Font&          _font,
	                                 const std::string&       _promptTitle,
	                                 spss::Function<std::any> _onConfirm,
	                                 spss::Function<std::any> _onCancel,
	                                 const unsigned int       _charSize,
	                                 const std::string&       _defaultStr)
	            : m_window{nullptr},
	              m_lastMousePosition{},
	              m_dragging{false},
	              m_rect{{_size.x - 1 >= MIN_WIDTH ? _size.x - 1 : MIN_WIDTH,
	                      _size.y - 1 >= MIN_HEIGHT ? _size.y - 1 : MIN_HEIGHT}},
	              m_textEntry{_size.x - 1 >= MIN_WIDTH ? _size.x - 1 : MIN_WIDTH,
	                          _position,
	                          _font,
	                          _charSize,
	                          _defaultStr},
	              m_lastPosition{_position},
	              m_confirmAction{_onConfirm},
	              m_cancelAction{_onCancel},
	              m_alignmentNeeded{true} {
		m_title.setFont(_font);
		m_title.setString(_promptTitle);

		m_confirmText.setCharacterSize(17);
		m_confirmText.setFont(_font);
		m_confirmText.setString("Ok");

		m_cancelText.setCharacterSize(17);
		m_cancelText.setFont(_font);
		m_cancelText.setString("Cancel");

		m_rect.setFillColor({0, 0, 0, 100});
		m_rect.setOutlineThickness(1);

		setPosition(_position);
		m_textEntry.setAlwaysActive(true);

		m_confirmButton.setSize({100, 30});
		m_cancelButton.setSize({100, 30});

		m_confirmButton.setFillColor(sf::Color::Transparent);
		m_cancelButton.setFillColor(sf::Color::Transparent);

		m_confirmButton.setOutlineColor(UNHIGHLIGHTED_BORDER);
		m_cancelButton.setOutlineColor(UNHIGHLIGHTED_BORDER);

		m_confirmButton.setOutlineThickness(-2);
		m_cancelButton.setOutlineThickness(-2);

		m_confirmButton.setOrigin(m_confirmButton.getGlobalBounds().left +
		                            m_confirmButton.getGlobalBounds().width / 2,
		                          m_confirmButton.getGlobalBounds().top +
		                            m_confirmButton.getGlobalBounds().height / 2);

		m_cancelButton.setOrigin(m_cancelButton.getGlobalBounds().left +
		                           m_cancelButton.getGlobalBounds().width / 2,
		                         m_cancelButton.getGlobalBounds().top +
		                           m_cancelButton.getGlobalBounds().height / 2);

		m_confirmText.setOrigin(m_confirmText.getGlobalBounds().left +
		                          m_confirmText.getGlobalBounds().width / 2,
		                        m_confirmText.getGlobalBounds().top +
		                          m_confirmText.getGlobalBounds().height / 2);

		m_cancelText.setOrigin(m_cancelText.getGlobalBounds().left +
		                         m_cancelText.getGlobalBounds().width / 2,
		                       m_cancelText.getGlobalBounds().top +
		                         m_cancelText.getGlobalBounds().height / 2);

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
		window.draw(m_confirmButton, states);
		window.draw(m_confirmText, states);
		if (m_cancelAction != nullptr) {
			window.draw(m_cancelButton, states);
			window.draw(m_cancelText, states);
		}
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

	void TextEntryPrompt::setColor(const sf::Color& _color) {
		m_rect.setFillColor(_color);
		m_textEntry.setBackgroundColor(_color);
	}

	void TextEntryPrompt::handleMouseClick() {
		auto view{m_window->getView()};
		auto mousePos = sf::Mouse::getPosition(*m_window);
		auto pixelPos{m_window->mapPixelToCoords(mousePos, view)};

		auto bounds{m_rect.getGlobalBounds()};
		auto confirmButtonBounds{m_confirmButton.getGlobalBounds()};
		auto cancelButtonBounds{m_cancelButton.getGlobalBounds()};

		bool insideBox{bounds.contains(pixelPos.x, pixelPos.y)};
		bool insideConfirmButton{confirmButtonBounds.contains(pixelPos.x, pixelPos.y)};
		bool insideCancelButton{cancelButtonBounds.contains(pixelPos.x, pixelPos.y)};

		if (insideBox && !insideConfirmButton && !insideCancelButton) {
			m_lastMousePosition = mousePos;
			m_dragging          = true;
		}

		if (insideConfirmButton && m_confirmAction != nullptr) {
			m_confirmAction(std::any());
		}

		else if (insideCancelButton && m_cancelAction != nullptr) {
			m_cancelAction(std::any());
		}
	}

	void TextEntryPrompt::handleMouseover() {
		auto view{m_window->getView()};
		auto mousePos = sf::Mouse::getPosition(*m_window);
		auto pixelPos{m_window->mapPixelToCoords(mousePos, view)};

		auto confirmButtonBounds{m_confirmButton.getGlobalBounds()};
		auto cancelButtonBounds{m_cancelButton.getGlobalBounds()};

		bool insideConfirmButton{confirmButtonBounds.contains(pixelPos.x, pixelPos.y)};
		bool insideCancelButton{cancelButtonBounds.contains(pixelPos.x, pixelPos.y)};

		if (insideConfirmButton && m_confirmAction != nullptr) {
			m_confirmButton.setOutlineColor(HIGHLIGHTED_BORDER);
		}
		else {
			m_confirmButton.setOutlineColor(UNHIGHLIGHTED_BORDER);
		}

		if (insideCancelButton && m_cancelAction != nullptr) {
			m_cancelButton.setOutlineColor(HIGHLIGHTED_BORDER);
		}
		else {
			m_cancelButton.setOutlineColor(UNHIGHLIGHTED_BORDER);
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

		auto titleHeight{m_title.getGlobalBounds().height};

		m_title.setPosition(pos);
		m_textEntry.setPosition({pos.x, pos.y + (2 * titleHeight)});

		float buttonsY{pos.y + size.y - m_confirmButton.getGlobalBounds().height};

		sf::Vector2f confirmPosition{pos.x + (size.x * 0.25F), buttonsY};
		sf::Vector2f cancelPosition{pos.x + (size.x * 0.75F), buttonsY};

		m_confirmButton.setPosition(confirmPosition);
		m_confirmText.setPosition(confirmPosition);

		m_cancelButton.setPosition(cancelPosition);
		m_cancelText.setPosition(cancelPosition);
	}

} // namespace spss
