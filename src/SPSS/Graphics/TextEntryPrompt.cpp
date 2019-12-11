#include <SPSS/Graphics/TextEntryPrompt.h>

namespace spss {
	TextEntryPrompt::TextEntryPrompt(const sf::Vector2f& _size,
	                                 const sf::Vector2f& _position,
	                                 const sf::Font&     _font,
	                                 const std::string&  _defaultStr,
	                                 unsigned            _charSize)
	            : m_infoBox{_size, _position, _font, _charSize},
	              m_textEntry{_font, _charSize, _size.x * 0.9F, _defaultStr},
	              m_lastPosition{_position} {
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

		//if (m_lastPosition != m_infoBox.getPosition()) {
			alignElements();
		//	m_lastPosition = m_infoBox.getPosition();
		//}
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

	void TextEntryPrompt::alignElements() {
		auto pos{m_infoBox.getPosition()};
		auto size{m_infoBox.getSize()};

		float xOffset{(size.x - m_textEntry.getWidth()) / 2};

		m_textEntry.setPosition({pos.x + xOffset, pos.y});

		m_confirmButton.setPosition({pos.x + (size.x * 0.25F), pos.y + (size.y * 0.75F)});
		m_cancelButton.setPosition({pos.x + (size.x * 0.75F), pos.y + (size.y * 0.75F)});
	}

} // namespace spss
