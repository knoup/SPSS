#ifndef MENULIST_H_INCLUDED
#define MENULIST_H_INCLUDED

#include <SPSS/Graphics/MulticolorText.h>
#include <SFML/Graphics.hpp>

#include <SPSS/System/MenuListMessage.h>
#include <SPSS/System/Message.h>

namespace spss {

	class MenuList : public sf::Drawable {
	  public:
		MenuList(const sf::Vector2f& _size,
				 const sf::Vector2f& _position,
				 const sf::Font&     _font,
				 unsigned            _charSize = 20);

		void appendMessage(const Message _msg);
		void getInput(sf::Event& _event);
		void update();
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		void setSize(const sf::Vector2f& _size);
		void setPosition(const sf::Vector2f& _pos);

	  private:
		//Functions -----------------------------------
		//
		//Message-related
		void positionMessage(int _index);
		float getLineSpacing() const;
		//View-related
		void  onResize(sf::Vector2u _newSize);
		void  snapToTop();
		void  snapToBottom();
		float getUpperViewBound() const;
		float getLowerViewBound() const;
		bool  viewAtHighest() const;
		bool  viewAtLowest() const;
		void  scrollUp();
		void  scrollDown();
		//---------------------------------------------

		//Data members --------------------------------
		mutable sf::RenderTarget* m_target;
		sf::Vector2u              m_lastTargetSize;

		sf::Vector2f       m_size;
		sf::Vector2f       m_position;
		const sf::Font&    m_font;
		unsigned           m_charSize;

		sf::View           m_view;
		sf::View           m_shadedRectangleView;
		sf::RectangleShape m_shadedRectangle;

		std::vector<MenuListMessage> m_messages;
		//---------------------------------------------
	};
}

#endif // MENULIST_H_INCLUDED
