#ifndef DIALOGPROMPT_H_INCLUDED
#define DIALOGPROMPT_H_INCLUDED

#include <any>

#include <SPSS/Graphics/DrawableToWindow.h>
#include <SPSS/Graphics/TextEntryBox.h>
#include <SPSS/Util/Function.h>

namespace spss {
	class DialogPrompt : public spss::DrawableToWindow {
	  public:
		DialogPrompt(bool                _textEntryEnabled,
		             const sf::Vector2f& _position,
		             const sf::Font&     _font,
		             const std::string&  _promptTitle,
		             const unsigned int  _charSize   = 20,
		             const std::string&  _defaultStr = "");

		void getInput(sf::Event& _e);
		void update();
		void draw(sf::RenderWindow& window, sf::RenderStates states) const;

		void addButton(const std::string& _str, spss::Function<std::any> _action);

		const std::string getString() const;

		const sf::Vector2f& getPosition() const;
		const sf::Vector2f& getSize() const;

		const sf::FloatRect& getLocalBounds() const;
		const sf::FloatRect& getGlobalBounds() const;

		const sf::Color& getColor() const;

		void setPosition(const sf::Vector2f& _pos);
		void setOrigin(const sf::Vector2f& _origin);
		void setColor(const sf::Color& _color);

	  private:
		struct Button {
			sf::RectangleShape       m_shape;
			sf::Text                 m_text;
			spss::Function<std::any> m_action;

			void setPosition(const sf::Vector2f& _pos) {
				m_shape.setPosition(_pos);
				m_text.setPosition(_pos);
			}
		};

		void fitWidth(float _width);

		void setHeight(float _height);
		void setWidth(float _width);

		void handleMouseClick();
		void handleMouseover();

		void dragBox();
		void alignElements();

		mutable sf::RenderWindow* m_window;

		const sf::Font& m_font;
		sf::Text        m_title;

		sf::Vector2i m_lastMousePosition;
		bool         m_dragging;

		sf::RectangleShape            m_rect;
		std::unique_ptr<TextEntryBox> m_textEntry;

		sf::Vector2f m_lastPosition;

		std::vector<Button> m_buttons;

		bool m_alignmentNeeded;
	};
} // namespace spss

#endif // DIALOGPROMPT_H_INCLUDED
