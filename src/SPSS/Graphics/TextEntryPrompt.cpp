#include <SPSS/Graphics/TextEntryPrompt.h>

constexpr float MIN_WIDTH {200.F};
constexpr float MIN_HEIGHT{100.F};

namespace spss {
	TextEntryPrompt::TextEntryPrompt(const sf::Vector2f& _size,
	                                 const sf::Vector2f& _position,
	                                 const sf::Font&     _font,
	                                 const unsigned int  _charSize,
	                                 const std::string&  _defaultStr)
	            : m_infoBox{{_size.x >= MIN_WIDTH ? _size.x : MIN_WIDTH,
	                         _size.y >= MIN_HEIGHT ? _size.y : MIN_HEIGHT},
	                         _position,
	                         _font,
	                         _charSize},
	              m_textEntry{_size.x >= MIN_WIDTH ? _size.x : MIN_WIDTH,
	                          _position,
	                          _font,
	                          _charSize,
	                          _defaultStr},
				  m_lastPosition{_position},
				  m_origin{0.F, 0.F},
				  m_alignmentNeeded{true} {
		m_infoBox.setColor(sf::Color::Black);
		m_infoBox.setDraggable(true);
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
		m_infoBox.getInput(_e);
		m_textEntry.getInput(_e);
	}

	void TextEntryPrompt::update() {
		m_infoBox.update();
		m_textEntry.update();

		if (m_infoBox.getPosition() != m_lastPosition) {
			m_lastPosition = m_infoBox.getPosition();
			m_alignmentNeeded = true;
		}

		alignElements();
	}

	void TextEntryPrompt::draw(sf::RenderWindow& window, sf::RenderStates states) const {
		window.draw(m_infoBox, states);
		window.draw(m_textEntry, states);
		window.draw(m_confirmButton, states);
		window.draw(m_cancelButton, states);
	}
	void TextEntryPrompt::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		sf::RenderWindow* w{dynamic_cast<sf::RenderWindow*>(&target)};
		if (w == nullptr) {
			return;
		}

		draw(*w, states);
	}

	const std::string& TextEntryPrompt::getString() const {
		return m_textEntry.getCurrentString();
	}

	const sf::Vector2f& TextEntryPrompt::getPosition() const {
		return m_infoBox.getPosition();
	}

	const sf::Vector2f& TextEntryPrompt::getSize() const {
		return m_infoBox.getSize();
	}

	const sf::FloatRect& TextEntryPrompt::getLocalBounds() const {
		return m_infoBox.getLocalBounds();
	}

	const sf::FloatRect& TextEntryPrompt::getGlobalBounds() const {
		return m_infoBox.getGlobalBounds();
	}

	void TextEntryPrompt::setPosition(const sf::Vector2f& _pos) {
		m_infoBox.setPosition(_pos);

		m_alignmentNeeded = true;
	}

	void TextEntryPrompt::setSize(const sf::Vector2f& _size) {
		float w{_size.x >= MIN_WIDTH ? _size.x : MIN_WIDTH};
		float h{_size.y >= MIN_HEIGHT ? _size.y : MIN_HEIGHT};

		m_infoBox.setSize({w, h});
		m_textEntry.setWidth(w);

		m_alignmentNeeded = true;
	}

	void TextEntryPrompt::setOrigin(const sf::Vector2f& _origin) {
		m_origin = _origin;

		m_alignmentNeeded = true;
	}

	void TextEntryPrompt::alignElements() {
		if (!m_alignmentNeeded) {
			return;
		}

		m_alignmentNeeded = false;

		auto pos{m_infoBox.getPosition()};
		pos += m_origin;
		auto size{m_infoBox.getSize()};

		m_textEntry.setPosition(pos);

		m_confirmButton.setPosition({pos.x + (size.x * 0.25F), pos.y + (size.y * 0.75F)});
		m_cancelButton.setPosition({pos.x + (size.x * 0.75F), pos.y + (size.y * 0.75F)});
	}

} // namespace spss
