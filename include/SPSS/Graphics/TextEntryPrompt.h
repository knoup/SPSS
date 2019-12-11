#ifndef TEXTENTRYPROMPT_H_INCLUDED
#define TEXTENTRYPROMPT_H_INCLUDED

#include <SPSS/Graphics/InfoBox.h>
#include <SPSS/Graphics/TextEntryBox.h>

namespace spss {
	class TextEntryPrompt : public sf::Drawable {
	  public:
		TextEntryPrompt(const sf::Vector2f& _size,
		                const sf::Vector2f& _position,
		                const sf::Font&     _font,
		                const std::string&  _defaultStr = "",
		                unsigned            _charSize   = 20);

		void getInput(sf::Event& _e);
		void update();
		void draw(sf::RenderWindow& window, sf::RenderStates states) const;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	  private:
		void alignElements();

		InfoBox            m_infoBox;
		TextEntryBox       m_textEntry;
		sf::RectangleShape m_confirmButton;
		sf::RectangleShape m_cancelButton;
	};
} // namespace spss

#endif // TEXTENTRYPROMPT_H_INCLUDED