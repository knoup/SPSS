#include "SPSS/System/MenuList.h"

#include "FontManager.h"
#include "InputLocker.h"
#include "Keybinds.h"

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
				  m_view{},
				  m_shadedRectangleView{},
				  m_shadedRectangle{},
				  m_messages{} {
		m_shadedRectangle.setOutlineColor(sf::Color(255, 165, 0));
		m_shadedRectangle.setFillColor(sf::Color(0, 0, 0, 100));
	}

	void MenuList::appendMessage(const Message _msg) {
		MenuListMessage newMessage{_msg, m_font, m_charSize};

		newMessage.fitWidth(m_view.getSize().x * 0.9f);

		m_messages.push_back(newMessage);
		positionMessage(m_messages.size() - 1);
	}

	void MenuList::getInput(sf::Event& _event) {

	}

	void MenuList::update() {
		//onResize() will need to be called at least once after
		//m_window is set, so this code will be needed here.
		//As a result, we also won't really need to detect
		//a Resized event in getInput(), since this part
		//will handle that as well.
		if (m_window != nullptr) {
			if (m_window->getSize() != m_lastWindowSize) {
				m_lastWindowSize = m_window->getSize();
				onResize(m_lastWindowSize);
			}
		}
	}

	void MenuList::draw(sf::RenderWindow& window, sf::RenderStates states) const {
		m_window              = &window;
		sf::View previousView = m_window->getView();

		m_window->setView(m_shadedRectangleView);
		m_window->draw(m_shadedRectangle, states);

		m_window->setView(m_view);
		for (const auto& message : m_messages) {
			m_window->draw(message, states);
		}
		m_window->setView(previousView);
	}

	void MenuList::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		sf::RenderWindow* w{dynamic_cast<sf::RenderWindow*>(&target)};
		if(w == nullptr) {
			return;
		}

		draw(*w, states);
	}

	void MenuList::setSize(const sf::Vector2f& _size) {
		m_size = _size;
	}

	void MenuList::setPosition(const sf::Vector2f& _pos) {
		m_position = _pos;
	}


	//This function sets the position of a new message, at the
	//very bottom of the box
	void MenuList::positionMessage(int _index) {
		if (!m_messages.empty() && _index > 0) {
			MenuListMessage& message = m_messages[_index];
			sf::Vector2f    newPosition{m_messages[_index - 1].getPosition()};
			unsigned int    lastMessageLines{
			  m_messages[_index - 1].getNumberOfLines()};
			newPosition.y += lastMessageLines * (getLineSpacing());
			message.setPosition(newPosition);
		}
	}

	float MenuList::getLineSpacing() const {
		return m_font.getLineSpacing(m_charSize);
	}


	void MenuList::onResize(sf::Vector2u _newSize) {
		float left{m_position.x / _newSize.x};
		float top{m_position.y / _newSize.y};
		float width{m_size.x / _newSize.x};
		float height{m_size.y / _newSize.y};

		sf::FloatRect viewRect{0, 0, _newSize.x * width, _newSize.y * height};
		sf::FloatRect viewPort{left, top, width, height};

		m_view.reset(viewRect);
		m_shadedRectangleView.reset(viewRect);

		m_view.setViewport(viewPort);
		m_shadedRectangleView.setViewport(viewPort);

		m_shadedRectangle.setSize(m_shadedRectangleView.getSize());

		for (size_t i{0}; i < m_messages.size(); i++) {
			MenuListMessage& message = m_messages[i];
			message.fitWidth(m_view.getSize().x * 0.9f);
			positionMessage(i);
		}
	}

	//This function checks if the last message is "outside" (below) the view.
	//If so, it adjusts the view's center so that the very first message is
	//on top.
	void MenuList::snapToTop() {
		if (!m_messages.empty()) {
			MenuListMessage& latestMessage = m_messages.back();

			float lastLineYPosition{latestMessage.getPosition().y +
									(latestMessage.getNumberOfLines() - 1) *
									  getLineSpacing()};

			float boundary{m_view.getSize().y};

			if (lastLineYPosition < boundary) {
				return;
			}

			MenuListMessage& earliestMessage = m_messages.front();
			sf::Vector2f    newCenter{m_view.getCenter().x,
								   earliestMessage.getPosition().y};

			newCenter.y += (m_view.getSize().y / 2);

			m_view.setCenter(newCenter);
		}
	}

	//This function checks if the last message is "outside" (below) the view.
	//If so, it adjusts the view's center so that it is visible.
	void MenuList::snapToBottom() {
		if (!m_messages.empty()) {
			MenuListMessage& latestMessage = m_messages.back();
			float           lastLineYPosition{latestMessage.getPosition().y +
									(latestMessage.getNumberOfLines() - 1) *
									  getLineSpacing()};

			float boundary{m_view.getSize().y};

			if (lastLineYPosition < boundary) {
				return;
			}

			sf::Vector2f newCenter{m_view.getCenter().x, lastLineYPosition};

			newCenter.y -= (m_view.getSize().y / 2);

			m_view.setCenter(newCenter);
		}
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

	void MenuList::scrollUp() {
		if (!viewAtHighest()) {
			m_view.setCenter(m_view.getCenter().x,
							 m_view.getCenter().y - (getLineSpacing()));
		}
		if (viewAtHighest()) {
			snapToTop();
		}
	}

	void MenuList::scrollDown() {
		if (!viewAtLowest()) {
			m_view.setCenter(m_view.getCenter().x,
							 m_view.getCenter().y + (getLineSpacing()));
		}
		if (viewAtLowest()) {
			snapToBottom();
		}
	}

}
