#ifndef SCROLLBAR_H_INCLUDED
#define SCROLLBAR_H_INCLUDED

#include <SFML/Graphics.hpp>

namespace spss {

	class Scrollbar : public sf::Drawable {

	  public:

		Scrollbar(sf::RenderWindow*   _window,
			      const sf::View&     _backgroundView,
			      sf::View&           _scrollView,
			      const sf::Vector2f& _size = {},
			      const sf::Vector2f& _position = {},
				  float               _min = 0,
				  float               _max = 0);

		void reset(const sf::Vector2f& _size,
			       const sf::Vector2f& _position,
				   float               _min,
				   float               _max);

		void setActive(bool _b);

		void getInput(sf::Event& _event);
		void update();

		void draw(sf::RenderWindow& window, sf::RenderStates states) const;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		void setColor(sf::Color _c);

	  private:
	  	bool mousedOver() const;

	  	void scroll(bool _up);
	  	void drag();
	  	void calculateNewViewCenter();


	  	mutable sf::RenderWindow* m_window;
		const sf::View&           m_backgroundView;

		sf::View&          m_scrollView;

		sf::Color          m_color;       ///< The color of the scrollbar
		bool               m_active;      ///< Is the scrollbar active?
		mutable bool       m_dragging;    ///< Is the scrollbar being dragged?
		sf::RectangleShape m_outer;       ///< The outer part of the scrollbar
		sf::RectangleShape m_inner;       ///< The inner part of the scrollbar
		float              m_minRange;    ///< The minimum value for the scrollable view's center
		float              m_maxRange;    ///< The maximum value for the scrollable view's center

	};

}

#endif // SCROLLBAR_H_INCLUDED
