#include "SPSS/System/MenuList.h"

#include "FontManager.h"
#include "InputLocker.h"
#include "Keybinds.h"

constexpr float MIN_SIZE_X{100};
constexpr float MIN_SIZE_Y{150};

bool lmbPressed(sf::Event& _event) {
	if (_event.type == sf::Event::MouseButtonPressed &&
	    _event.mouseButton.button == sf::Mouse::Left) {
		return true;
	}
	else {
		return false;
	}
}

bool lmbReleased(sf::Event& _event) {
	if (_event.type == sf::Event::MouseButtonReleased &&
	    _event.mouseButton.button == sf::Mouse::Left) {
		return true;
	}
	else {
		return false;
	}
}

namespace spss {

	MenuList::MenuList(const sf::Vector2f& _size,
	                   const sf::Vector2f& _position,
	                   const sf::Font&     _font,
	                   unsigned            _charSize)
	            : m_window{nullptr},
	              m_lastWindowSize{},
	              m_size{_size},
	              m_position{_position},
	              m_font{_font},
	              m_charSize{_charSize},
	              m_resizing{false},
	              m_draggable{false},
	              m_dragging{false},
	              m_lastMousePosition{},
	              m_view{},
	              m_shadedRectangleView{},
	              m_shadedRectangle{},
	              m_messages{},
	              m_scrollbarColor{sf::Color::White},
	              m_scrollbarActive{false},
	              m_scrollbarDragging{false},
	              m_scrollbarMinRange{0},
	              m_scrollbarMaxRange{0} {
		m_shadedRectangle.setOutlineColor(sf::Color(255, 165, 0));
		m_shadedRectangle.setFillColor(sf::Color(0, 0, 0, 100));

		//m_resizeStrip is going to be angled 45 degrees at the bottom
		//of the scrollbar (if present); i.e., it will be the hypotenuse
		//of a right triangle with the two other sides being 15.
		//Pythagoras saves the day and gives us 21!

		m_resizeStrip.setRotation(45);
		m_resizeStrip.setSize({1, 21});
		m_resizeStrip.setFillColor({sf::Color::White});
	}

	void MenuList::appendMessage(const Message _msg) {
		MenuListMessage newMessage{_msg, m_font, m_charSize};

		newMessage.fitWidth(getUsableWidth());

		m_messages.push_back(newMessage);
		positionMessage(m_messages.size() - 1);
		adjustScrollbar();
	}

	void MenuList::getInput(sf::Event& _event) {
		if (m_window == nullptr) {
			return;
		}

		detectResizeStripInteractions(_event);
		detectScrollbarInteractions(_event);
		detectMenulistInteractions(_event);
	}

	void MenuList::update() {
		//reset() will need to be called at least once after
		//m_window is set, so this code will be needed here.
		//As a result, we also won't really need to detect
		//a Resized event in getInput(), since this part
		//will handle that as well.
		if (m_window == nullptr) {
			return;
		}

		if (m_window->getSize() != m_lastWindowSize) {
			m_lastWindowSize = m_window->getSize();
			reset();
		}

		resizeMenu();
		dragMenu();
		updateScrollbar();
	}

	void MenuList::draw(sf::RenderWindow& window, sf::RenderStates states) const {
		m_window              = &window;
		sf::View previousView = m_window->getView();

		m_window->setView(m_shadedRectangleView);
		m_window->draw(m_shadedRectangle, states);
		m_window->draw(m_resizeStrip, states);

		if (m_scrollbarActive) {
			m_window->draw(m_scrollbarOuter);
			m_window->draw(m_scrollbarInner);
		}

		m_window->setView(m_view);
		for (const auto& message : m_messages) {
			m_window->draw(message, states);
		}
		m_window->setView(previousView);
	}

	void MenuList::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		sf::RenderWindow* w{dynamic_cast<sf::RenderWindow*>(&target)};
		if (w == nullptr) {
			return;
		}

		draw(*w, states);
	}

	void MenuList::setSize(const sf::Vector2f& _size) {
		if (m_window == nullptr) {
			return;
		}

		m_size = _size;

		if (m_size.x < MIN_SIZE_X) {
			m_size.x = MIN_SIZE_X;
		}
		else if (m_size.x > m_window->getSize().x) {
			m_size.x = m_window->getSize().x;
		}

		if (m_size.y < MIN_SIZE_Y) {
			m_size.y = MIN_SIZE_Y;
		}
		else if (m_size.y > m_window->getSize().y) {
			m_size.y = m_window->getSize().y;
		}
	}

	void MenuList::setPosition(const sf::Vector2f& _pos) {
		if (m_window == nullptr) {
			return;
		}

		m_position = _pos;

		if (m_position.x < 0) {
			m_position.x = 0;
		}
		else if (m_position.x + m_size.x > m_window->getSize().x) {
			m_position.x = m_window->getSize().x - m_size.x;
		}

		if (m_position.y < 0) {
			m_position.y = 0;
		}
		else if (m_position.y + m_size.y > m_window->getSize().y) {
			m_position.y = m_window->getSize().y - m_size.y;
		}
	}

	void MenuList::setDraggable(bool _d) {
		m_draggable = _d;
	}

	bool MenuList::menuMousedOver() const {
		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
		sf::Vector2f pixelPos{m_window->mapPixelToCoords(mousePos, m_shadedRectangleView)};

		if (m_shadedRectangle.getGlobalBounds().contains(pixelPos.x, pixelPos.y)) {
			return true;
		}

		return false;
	}

	bool MenuList::scrollbarMousedOver() const {
		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
		sf::Vector2f pixelPos{m_window->mapPixelToCoords(mousePos, m_shadedRectangleView)};

		if (m_scrollbarOuter.getGlobalBounds().contains(pixelPos.x, pixelPos.y)) {
			return true;
		}

		return false;
	}

	bool MenuList::resizeStripMousedOver() const {
		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
		sf::Vector2f pixelPos{m_window->mapPixelToCoords(mousePos, m_shadedRectangleView)};

		//We'll want to see if the resize strip is being held, i.e. the 15 x 15 area
		//on the bottom right of the MenuList.

		if (m_resizeStrip.getGlobalBounds().contains(pixelPos.x, pixelPos.y)) {
			return true;
		}

		return false;
	}

	//This function sets the position of a new message, at the
	//very bottom of the box
	void MenuList::positionMessage(int _index) {
		if (!m_messages.empty() && _index > 0) {
			MenuListMessage& message = m_messages[_index];
			sf::Vector2f     newPosition{m_messages[_index - 1].getPosition()};
			unsigned int     lastMessageLines{
              m_messages[_index - 1].getNumberOfLines()};
			newPosition.y += lastMessageLines * (getLineSpacing());
			message.setPosition(newPosition);
		}
	}

	float MenuList::getLineSpacing() const {
		return m_font.getLineSpacing(m_charSize);
	}

	void MenuList::reset() {
		setPosition(m_position);
		setSize(m_size);

		float left{m_position.x / m_lastWindowSize.x};
		float top{m_position.y / m_lastWindowSize.y};
		float width{m_size.x / m_lastWindowSize.x};
		float height{m_size.y / m_lastWindowSize.y};

		sf::FloatRect viewRect{0, 0, m_lastWindowSize.x * width, m_lastWindowSize.y * height};
		sf::FloatRect viewPort{left, top, width, height};

		m_view.reset(viewRect);
		m_shadedRectangleView.reset(viewRect);

		m_view.setViewport(viewPort);
		m_shadedRectangleView.setViewport(viewPort);

		m_shadedRectangle.setSize(m_shadedRectangleView.getSize());

		for (size_t i{0}; i < m_messages.size(); i++) {
			MenuListMessage& message = m_messages[i];
			message.fitWidth(getUsableWidth());
			positionMessage(i);
		}

		adjustScrollbar();

		sf::Vector2f resizeStripPos{m_shadedRectangle.getGlobalBounds().width,
		                            m_shadedRectangle.getGlobalBounds().height};
		resizeStripPos.y -= 15;
		m_resizeStrip.setPosition(resizeStripPos);
	}

	float MenuList::getUpperViewBound() const {
		return 0;
	}
	float MenuList::getLowerViewBound() const {
		if (m_messages.empty()) {
			return 0;
		}
		float lowestPoint{m_messages.back().getPosition().y};
		return lowestPoint;
	}

	bool MenuList::viewAtHighest() const {
		if (m_view.getCenter().y - m_view.getSize().y / 2 <= getUpperViewBound()) {
			return true;
		}

		return false;
	}

	bool MenuList::viewAtLowest() const {
		if (m_view.getCenter().y + m_view.getSize().y / 2 >= getLowerViewBound()) {
			return true;
		}

		return false;
	}

	void MenuList::scroll(bool _up) {
		if (!m_scrollbarActive) {
			return;
		}

		auto  pos{m_scrollbarInner.getPosition()};
		float min{m_scrollbarOuter.getPosition().y};
		float max{m_scrollbarOuter.getPosition().y + m_scrollbarOuter.getGlobalBounds().height};
		max -= m_scrollbarInner.getGlobalBounds().height;

		if (_up) {
			pos.y -= m_scrollbarInner.getGlobalBounds().height / 2;
		}
		else {
			pos.y += m_scrollbarInner.getGlobalBounds().height / 2;
		}

		if (pos.y <= min) {
			pos.y = min;
		}
		else if (pos.y >= max) {
			pos.y = max;
		}

		m_scrollbarInner.setPosition(pos);
	}

	float MenuList::getUsableWidth() const {
		float scrollbarWidth{0};
		if (m_scrollbarActive) {
			scrollbarWidth = m_scrollbarOuter.getGlobalBounds().width;
		}
		return m_view.getSize().x - scrollbarWidth;
	}

	float MenuList::getMenuHeight() const {
		if (m_messages.empty()) {
			return 0;
		}

		auto firstTextPos{m_messages[0].getPosition()};
		auto lastTextPos{m_messages.back().getPosition()};

		auto firstTextBounds{m_messages[0].getGlobalBounds()};
		auto lastTextBounds{m_messages.back().getGlobalBounds()};

		//Since messages have a Y origin of 0, we'll add the last text's height

		return (lastTextPos.y + lastTextBounds.height) -
		       firstTextPos.y;
	}

	void MenuList::updateScrollbar() {
		if (!m_scrollbarActive) {
			return;
		}
		dragScrollbar();
		calculateNewScrollbarCenter();
	}

	void MenuList::adjustScrollbar() {
		if (m_messages.empty()) {
			return;
		}

		if (getMenuHeight() < m_size.y) {
			m_scrollbarActive = false;
			return;
		}

		m_scrollbarActive = true;

		//visibleHeight is essentially the height of the menu. However,
		//trueHeight will be used for all scrollbar size related
		//calculations.
		//
		//We'll subtract 15 and work under the assumption that the
		//scrollbar won't take up all vertical space; there will be
		//a little extra space at the bottom to be used for resizing.
		float visibleHeight{m_size.y};
		float trueHeight{m_size.y - 15};

		//Adjust the ranges so that they don't leave excess empty space.
		//We'll add a little extra space (linespacing / 2) at the end to
		//ensure no text is cut off.
		m_scrollbarMinRange = m_size.y / 2;
		m_scrollbarMaxRange = getMenuHeight() - (visibleHeight / 2) + getLineSpacing() / 2;

		//Initialize the outer scrollbar
		m_scrollbarOuter.setFillColor(sf::Color::Transparent);
		m_scrollbarOuter.setOutlineColor(m_scrollbarColor);
		m_scrollbarOuter.setOutlineThickness(-1);
		m_scrollbarOuter.setSize({15, trueHeight});

		float scrollbarX{m_shadedRectangleView.getCenter().x};
		scrollbarX += m_shadedRectangleView.getSize().x / 2;
		scrollbarX -= m_scrollbarOuter.getGlobalBounds().width;

		m_scrollbarOuter.setPosition({scrollbarX, 0});

		//Initialize the inner scrollbar

		//The ratio of inner height  :  outer height (i.e. trueHeight)
		//should equal
		//the ratio of trueHeight :  total menu height
		//so that the inner scrollbar's height scales linearly with the
		//amount of visible content
		float ratio{trueHeight / getMenuHeight()};
		m_scrollbarInner.setFillColor(m_scrollbarColor);
		m_scrollbarInner.setSize({15, trueHeight * ratio});

		//In order to avoid resetting the inner Y when unneccessary, we'll
		//keep it and adjust it only if it goes out of range.
		float innerY{m_scrollbarInner.getPosition().y};
		float maxY{m_scrollbarOuter.getPosition().y};
		maxY += m_scrollbarOuter.getGlobalBounds().height;
		maxY -= m_scrollbarInner.getGlobalBounds().height;
		if (innerY > maxY) {
			innerY = maxY;
		}

		m_scrollbarInner.setPosition({scrollbarX, innerY});
	}

	void MenuList::dragScrollbar() {
		if (!m_scrollbarDragging) {
			return;
		}

		auto pos{sf::Mouse::getPosition(*m_window)};
		auto mousePos{m_window->mapPixelToCoords(pos, m_shadedRectangleView)};

		//Just some aliases for readability
		auto outerPos{m_scrollbarOuter.getPosition()};
		auto outerBounds{m_scrollbarOuter.getGlobalBounds()};
		auto innerPos{m_scrollbarInner.getPosition()};
		auto innerBounds{m_scrollbarInner.getGlobalBounds()};

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

		m_scrollbarInner.setPosition(innerPos);
	}

	//TODO: this needs to be optimized. It's currently
	//taking too much processing power while dragging and
	//making the rest of the program choppy.
	void MenuList::dragMenu() {
		if (m_resizing || !m_dragging || m_window == nullptr) {
			return;
		}
		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);

		sf::Vector2i diff{mousePos.x - m_lastMousePosition.x,
		                  mousePos.y - m_lastMousePosition.y};

		if (m_lastMousePosition == mousePos) {
			return;
		}

		m_lastMousePosition = mousePos;

		sf::Vector2f newPos{m_position};
		newPos.x += diff.x;
		newPos.y += diff.y;
		setPosition(newPos);

		reset();
	}

	void MenuList::resizeMenu() {
		if (!m_resizing || m_window == nullptr) {
			return;
		}

		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);

		sf::Vector2i diff{mousePos.x - m_lastMousePosition.x,
		                  mousePos.y - m_lastMousePosition.y};

		if (m_lastMousePosition == mousePos) {
			return;
		}

		m_lastMousePosition = mousePos;

		sf::Vector2f newSize{m_size};
		newSize.x += diff.x;
		newSize.y += diff.y;
		setSize(newSize);

		reset();
	}

	void MenuList::detectMenulistInteractions(sf::Event& _event) {
		if (!m_draggable || m_scrollbarDragging) {
			return;
		}

		if (lmbPressed(_event)) {
			sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);

			if (menuMousedOver()) {
				m_lastMousePosition = mousePos;
				m_dragging          = true;
			}
		}

		else if (lmbReleased(_event)) {
			m_dragging = false;
		}
	}

	void MenuList::detectScrollbarInteractions(sf::Event& _event) {
		if (lmbPressed(_event) && scrollbarMousedOver()) {
			m_scrollbarDragging = true;
		}
		else if (lmbReleased(_event)) {
			m_scrollbarDragging = false;
		}

		if (_event.type == sf::Event::MouseWheelMoved && menuMousedOver()) {
			if (_event.mouseWheel.delta > 0) {
				scroll(true);
			}
			else if (_event.mouseWheel.delta < 0) {
				scroll(false);
			}
		}
	}

	void  MenuList::detectResizeStripInteractions(sf::Event& _event) {
		if (lmbPressed(_event) && resizeStripMousedOver()) {
			m_resizing = true;
		}
		else if (lmbReleased(_event)) {
			m_resizing = false;
		}
	}

	void MenuList::calculateNewScrollbarCenter() {
		if (!m_scrollbarActive) {
			return;
		}

		float innerY{m_scrollbarInner.getPosition().y};

		//First, we'll calculate the decimal percentage (between 0 and 1)
		//of the progress of innerY from its minimum visible value, the
		//value of outer scrollbar's Y position, to its maximum visible
		//value, that plus the height of the outer scrollbar, minus the
		//height of the inner.
		float minY{m_scrollbarOuter.getPosition().y};
		float maxY{minY + m_scrollbarOuter.getGlobalBounds().height};
		maxY -= m_scrollbarInner.getGlobalBounds().height;

		//The formula is: percentage = (value - min) / (max - min)
		float p{(innerY - minY) / (maxY - minY)};

		//Now, we'll calculate the value of that percentage in terms
		//of the center value ranges.
		//0 corresponds to the m_scrollbarMinRange, and
		//1 corresponds to the m_scrollbarMaxRange
		//
		//The formula is: value = ((max - min) * percentage) + min
		float newCenterY{((m_scrollbarMaxRange - m_scrollbarMinRange) * p) + m_scrollbarMinRange};

		auto c{m_view.getCenter()};
		c.y = newCenterY;
		m_view.setCenter(c);
	}
} // namespace spss
