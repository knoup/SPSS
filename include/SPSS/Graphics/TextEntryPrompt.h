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
		                const unsigned int  _charSize   = 20,
		                const std::string&  _defaultStr = "");

		void getInput(sf::Event& _e);
		void update();
		void draw(sf::RenderWindow& window, sf::RenderStates states) const;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		const std::string& getString() const;

		const sf::Vector2f& getPosition() const;
		const sf::Vector2f& getSize() const;

		const sf::FloatRect& getLocalBounds() const;
		const sf::FloatRect& getGlobalBounds() const;

		void setPosition(const sf::Vector2f& _pos);
		void setSize(const sf::Vector2f& _size);
		void setOrigin(const sf::Vector2f& _origin);

	  private:
		void alignElements();

		InfoBox            m_infoBox;
		TextEntryBox       m_textEntry;

		sf::Vector2f       m_lastPosition;

		sf::Vector2f       m_origin;
		sf::RectangleShape m_confirmButton;
		sf::RectangleShape m_cancelButton;

		bool               m_alignmentNeeded;
	};
} // namespace spss

#endif // TEXTENTRYPROMPT_H_INCLUDED
