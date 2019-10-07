#include <SPSS/System/Scrollbar.h>
#include <SPSS/Util/Input.h>

namespace spss {

	Scrollbar::Scrollbar(sf::RenderWindow*   _window,
						 const sf::View&     _backgroundView,
						 sf::View&           _scrollView,
						 const sf::Vector2f& _size,
						 const sf::Vector2f& _position,
						 float               _min,
						 float               _max) :
		m_window{_window},
		m_backgroundView{_backgroundView},
		m_scrollView{_scrollView},
		m_color{sf::Color::White},
		m_active{false},
		m_dragging{false},
		m_outer{},
		m_inner{},
		m_minRange{_min},
		m_maxRange{_max} {
			m_outer.setOutlineThickness(-1);
			m_outer.setFillColor(sf::Color::Transparent);
			setColor(m_color);
			reset(_size, _position, _min, _max);
	}



	void Scrollbar::reset(const sf::Vector2f& _size,
						  const sf::Vector2f& _position,
						  float               _min,
						  float               _max) {

		m_minRange = _min;
		m_maxRange = _max;

		m_outer.setSize(_size);
		m_outer.setPosition(_position);

		//Initialize the inner scrollbar
		float scrollbarHeight{_size.y};

		//The ratio of inner height  :  outer height (i.e. scrollbarHeight)
		//should equal
		//the ratio of scrollbarHeight :  total menu height
		//so that the inner scrollbar's height scales linearly with the
		//amount of visible content
		float ratio{scrollbarHeight / (m_maxRange - m_minRange)};
		m_inner.setSize({_size.x, scrollbarHeight * ratio});

		//In order to avoid resetting the inner Y when unneccessary, we'll
		//keep it and adjust it only if it goes out of range.
		float innerY{m_inner.getPosition().y};
		float maxY{m_outer.getPosition().y};
		maxY += m_outer.getGlobalBounds().height;
		maxY -= m_inner.getGlobalBounds().height;
		if (innerY > maxY) {
			innerY = maxY;
		}

		m_inner.setPosition({_position.x, innerY});
	}

	void Scrollbar::setActive(bool _b) {
		m_active = _b;
	}

	void Scrollbar::getInput(sf::Event& _event) {
		if (Util::Input::lmbPressed(_event) && mousedOver()) {
			m_dragging = true;
		}
		else if (Util::Input::lmbReleased(_event)) {
			m_dragging = false;
		}

		if (_event.type == sf::Event::MouseWheelMoved) {
			if (_event.mouseWheel.delta > 0) {
				scroll(true);
			}
			else if (_event.mouseWheel.delta < 0) {
				scroll(false);
			}
		}
	}

	void Scrollbar::update() {
		if (!m_active) {
			return;
		}
		drag();
		calculateNewViewCenter();
	}

	void Scrollbar::draw(sf::RenderWindow& window, sf::RenderStates states) const {
		m_window = &window;
		if (!m_active) {
			return;
		}
		m_window->draw(m_outer, states);
		m_window->draw(m_inner, states);
	}

	void Scrollbar::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		sf::RenderWindow* w{dynamic_cast<sf::RenderWindow*>(&target)};
		if (w == nullptr) {
			return;
		}

		draw(*w, states);
	}

	void Scrollbar::setColor(sf::Color _c) {
		m_outer.setOutlineColor(_c);
		m_inner.setFillColor(_c);
	}

	bool Scrollbar::mousedOver() const {
		if (m_window == nullptr) {
			return false;
		}

		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
		sf::Vector2f pixelPos{m_window->mapPixelToCoords(mousePos, m_backgroundView)};

		if (m_outer.getGlobalBounds().contains(pixelPos.x, pixelPos.y)) {
			return true;
		}

		return false;
	}

	void Scrollbar::scroll(bool _up) {
		if (!m_active) {
			return;
		}

		auto  pos{m_inner.getPosition()};
		float min{m_outer.getPosition().y};
		float max{m_outer.getPosition().y + m_outer.getGlobalBounds().height};
		max -= m_inner.getGlobalBounds().height;

		if (_up) {
			pos.y -= m_inner.getGlobalBounds().height / 2;
		}
		else {
			pos.y += m_inner.getGlobalBounds().height / 2;
		}

		if (pos.y <= min) {
			pos.y = min;
		}
		else if (pos.y >= max) {
			pos.y = max;
		}

		m_inner.setPosition(pos);
	}

	void Scrollbar::drag() {
		if (m_window == nullptr || !m_dragging) {
			return;
		}

		auto pos{sf::Mouse::getPosition(*m_window)};
		auto mousePos{m_window->mapPixelToCoords(pos, m_backgroundView)};

		//Just some aliases for readability
		auto outerPos{m_outer.getPosition()};
		auto outerBounds{m_outer.getGlobalBounds()};
		auto innerPos{m_inner.getPosition()};
		auto innerBounds{m_inner.getGlobalBounds()};

		float lowerLimit{outerPos.y};
		float upperLimit{outerPos.y + outerBounds.height - innerBounds.height};

		//We'll add half the inner height so that our cursor
		//is associated with the middle and not the top
		innerPos.y = mousePos.y - innerBounds.height / 2;

		if (innerPos.y <= lowerLimit) {
			innerPos.y = lowerLimit;
		}
		else if (innerPos.y >= upperLimit) {
			innerPos.y = upperLimit;
		}

		m_inner.setPosition(innerPos);
	}

	void Scrollbar::calculateNewViewCenter() {
		if (!m_active) {
			return;
		}

		float innerY{m_inner.getPosition().y};

		//First, we'll calculate the decimal percentage (between 0 and 1)
		//of the progress of innerY from its minimum visible value, the
		//value of outer scrollbar's Y position, to its maximum visible
		//value, that plus the height of the outer scrollbar, minus the
		//height of the inner.
		float minY{m_outer.getPosition().y};
		float maxY{minY + m_outer.getGlobalBounds().height};
		maxY -= m_inner.getGlobalBounds().height;

		//The formula is: percentage = (value - min) / (max - min)
		float p{(innerY - minY) / (maxY - minY)};

		//Now, we'll calculate the value of that percentage in terms
		//of the center value ranges.
		//0 corresponds to the m_minRange, and
		//1 corresponds to the m_maxRange
		//
		//The formula is: value = ((max - min) * percentage) + min
		float newCenterY{((m_maxRange - m_minRange) * p) + m_minRange};

		auto c{m_scrollView.getCenter()};
		c.y = newCenterY;
		m_scrollView.setCenter(c);
	}
}
