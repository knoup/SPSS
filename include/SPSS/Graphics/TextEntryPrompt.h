#ifndef TEXTENTRYPROMPT_H_INCLUDED
#define TEXTENTRYPROMPT_H_INCLUDED

#include <SPSS/Graphics/DrawableToWindow.h>
#include <SPSS/Graphics/InfoBox.h>
#include <SPSS/Graphics/TextEntryBox.h>

namespace spss {
	class TextEntryPrompt : public spss::DrawableToWindow {
	  public:
		TextEntryPrompt(const sf::Vector2f& _size,
		                const sf::Vector2f& _position,
		                const sf::Font&     _font,
		                const std::string&  _promptTitle,
		                const unsigned int  _charSize   = 20,
		                const std::string&  _defaultStr = "");

		void getInput(sf::Event& _e);
		void update();
		void draw(sf::RenderWindow& window, sf::RenderStates states) const;

		const std::string& getString() const;

		const sf::Vector2f& getPosition() const;
		const sf::Vector2f& getSize() const;

		const sf::FloatRect& getLocalBounds() const;
		const sf::FloatRect& getGlobalBounds() const;

		void setPosition(const sf::Vector2f& _pos);
		void setSize(const sf::Vector2f& _size);
		void setOrigin(const sf::Vector2f& _origin);

	  private:
	  	void dragBox();
		void alignElements();

		mutable sf::RenderWindow*  m_window;

		sf::Vector2i       m_lastMousePosition;
		bool               m_dragging;

		sf::RectangleShape m_rect;
		TextEntryBox       m_textEntry;

		sf::Vector2f       m_lastPosition;

		sf::RectangleShape m_confirmButton;
		sf::RectangleShape m_cancelButton;

		bool               m_alignmentNeeded;
	};
} // namespace spss

#endif // TEXTENTRYPROMPT_H_INCLUDED
