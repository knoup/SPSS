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
		void draw(sf::RenderWindow& window, sf::RenderStates states) const;
		//Since we only want MenuLists to be drawable on sf::RenderWindows, and
		//since sf::Drawable requires this function to be overloaded, we'll
		//dynamically cast target to a sf::RenderWindow. If that's not possible,
		//then draw will do nothing.
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		void setSize(const sf::Vector2f& _size);
		void setPosition(const sf::Vector2f& _pos);
		void setDraggable(bool _d);

	  private:
		//Functions -----------------------------------
		//
		bool menuMousedOver() const;
		bool scrollbarMousedOver() const;
		bool resizeStripMousedOver() const;

		void  positionMessage(int _index);
		float getLineSpacing() const;
		void  reset();
		float getUpperViewBound() const;
		float getLowerViewBound() const;
		bool  viewAtHighest() const;
		bool  viewAtLowest() const;

		void scroll(bool _up);

		//Gets the maximum width text items can be; if
		//scrollbar is active, it will be smaller
		float getUsableWidth() const;

		float getMenuHeight() const;
		void  updateScrollbar();
		void  adjustScrollbar();
		void  dragScrollbar();
		void  dragMenu();
		void  resizeMenu();
		void  detectMenulistInteractions(sf::Event& _event);
		void  detectScrollbarInteractions(sf::Event& _event);
		void  detectResizeStripInteractions(sf::Event& _event);
		void  calculateNewScrollbarCenter();
		//---------------------------------------------

		//Data members --------------------------------
		mutable sf::RenderWindow* m_window;
		sf::Vector2u              m_lastWindowSize;

		sf::Vector2f    m_size;
		sf::Vector2f    m_position;
		const sf::Font& m_font;
		unsigned        m_charSize;

		bool 		 m_resizing;

		bool         m_draggable;
		bool         m_dragging;
		sf::Vector2i m_lastMousePosition;

		sf::View           m_view;
		sf::View           m_shadedRectangleView;
		sf::RectangleShape m_shadedRectangle;
		sf::VertexArray    m_resizeStrip;

		std::vector<MenuListMessage> m_messages;

		sf::Color          m_scrollbarColor;
		bool               m_scrollbarActive;
		mutable bool       m_scrollbarDragging;
		sf::RectangleShape m_scrollbarOuter;
		sf::RectangleShape m_scrollbarInner;
		float              m_scrollbarMinRange;
		float              m_scrollbarMaxRange;
		//---------------------------------------------
	};
} // namespace spss

#endif // MENULIST_H_INCLUDED
