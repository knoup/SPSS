#include <SPSS/System/InfoBox.h>
#include <SPSS/Util/Input.h>

constexpr float MIN_SIZE_X{100};
constexpr float MIN_SIZE_Y{150};

constexpr float SCROLLBAR_WIDTH{15};

constexpr float RESIZESTRIP_WIDTH{15};
constexpr float RESIZESTRIP_HEIGHT{15};

namespace spss {

	////////////////////////////////////////////////////////////
	InfoBox::InfoBox(const sf::Vector2f& _size,
	                   const sf::Vector2f& _position,
	                   const sf::Font&     _font,
	                   unsigned            _charSize)
	            : m_window{nullptr},
	              m_lastWindowSize{},
	              m_size{_size},
	              m_position{_position},
	              m_font{_font},
	              m_charSize{_charSize},
	              m_resizable{false},
	              m_resizing{false},
	              m_draggable{false},
	              m_dragging{false},
	              m_lastMousePosition{},
	              m_view{},
	              m_shadedRectangleView{},
	              m_shadedRectangle{},
	              m_shadedRectangleColor{0, 0, 0, 100},
	              m_resizeStrip{sf::Triangles, 3},
	              m_messages{},
	              m_scrollbar{m_window, m_shadedRectangleView, m_view} {
		setColor(m_shadedRectangleColor);
		setScrollbarColor(sf::Color::White);
	}

	////////////////////////////////////////////////////////////
	void InfoBox::appendMessage(const Message _msg) {
		InfoBoxMessage newMessage{_msg, m_font, m_charSize};
		m_messages.push_back(newMessage);
		positionMessage(m_messages.size() - 1);
		setupScrollbar();
	}

	////////////////////////////////////////////////////////////
	void InfoBox::getInput(sf::Event& _event) {
		if (m_window == nullptr) {
			return;
		}

		detectResizeStripInteractions(_event);
		m_scrollbar.getInput(_event);
		detectBoxInteractions(_event);
	}

	////////////////////////////////////////////////////////////
	void InfoBox::update() {
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

		resizeBox();
		dragBox();
		m_scrollbar.update();
	}

	////////////////////////////////////////////////////////////
	void InfoBox::draw(sf::RenderWindow& window, sf::RenderStates states) const {
		m_window              = &window;
		sf::View previousView = m_window->getView();

		m_window->setView(m_shadedRectangleView);
		m_window->draw(m_shadedRectangle, states);

		if(m_resizable) {
			m_window->draw(m_resizeStrip, states);
		}

		m_window->draw(m_scrollbar, states);

		m_window->setView(m_view);
		for (const auto& message : m_messages) {
			m_window->draw(message, states);
		}
		m_window->setView(previousView);
	}

	////////////////////////////////////////////////////////////
	void InfoBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		sf::RenderWindow* w{dynamic_cast<sf::RenderWindow*>(&target)};
		if (w == nullptr) {
			return;
		}

		draw(*w, states);
	}

	////////////////////////////////////////////////////////////
	void InfoBox::setColor(sf::Color _color) {
		m_shadedRectangle.setFillColor(m_shadedRectangleColor);
	}

	////////////////////////////////////////////////////////////
	void InfoBox::setScrollbarColor(sf::Color _color) {
		m_scrollbar.setColor(_color);
	}

	////////////////////////////////////////////////////////////
	void InfoBox::setSize(const sf::Vector2f& _size) {
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

	////////////////////////////////////////////////////////////
	void InfoBox::setPosition(const sf::Vector2f& _pos) {
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

	////////////////////////////////////////////////////////////
	void InfoBox::setResizable(bool _r) {
		m_resizable = _r;
	}

	////////////////////////////////////////////////////////////
	void InfoBox::setDraggable(bool _d) {
		m_draggable = _d;
	}

	////////////////////////////////////////////////////////////
	bool InfoBox::boxMousedOver() const {
		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
		sf::Vector2f pixelPos{m_window->mapPixelToCoords(mousePos, m_shadedRectangleView)};

		auto bounds{m_shadedRectangle.getGlobalBounds()};

		if (bounds.contains(pixelPos.x, pixelPos.y)) {
			return true;
		}

		return false;
	}

	////////////////////////////////////////////////////////////
	bool InfoBox::resizeStripMousedOver() const {
		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
		sf::Vector2f pixelPos{m_window->mapPixelToCoords(mousePos, m_shadedRectangleView)};

		//We'll want to see if the resize strip is being held, i.e. the 15 x 15 area
		//on the bottom right of the InfoBox.

		sf::FloatRect region{m_shadedRectangle.getGlobalBounds().width - 15,
		                     m_shadedRectangle.getGlobalBounds().height - 15,
		                     15,
		                     15};

		if (region.contains(pixelPos.x, pixelPos.y)) {
			return true;
		}

		return false;
	}

	////////////////////////////////////////////////////////////
	void InfoBox::positionMessage(int _index) {
		if (!m_messages.empty()) {
			float lineSpacing{m_font.getLineSpacing(m_charSize)};

			InfoBoxMessage& msg{m_messages[_index]};
			msg.fitWidth(getUsableWidth());

			float lastMessageLines{0};
			sf::Vector2f newPosition{0, 0};

			if(_index > 0) {
				const InfoBoxMessage& lastMsg{m_messages[_index - 1]};
				lastMessageLines = lastMsg.getNumberOfLines();
				newPosition = lastMsg.getText().getPosition();
			}

			newPosition.y += (lastMessageLines * lineSpacing);
			msg.setPosition(newPosition);
		}
	}

	////////////////////////////////////////////////////////////
	void InfoBox::reset(bool _resized) {
		//First, we'll initialise the views and the shaded
		//rectangle
		//////////////////////////////////////////////////
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


		//Next, we'll initialise the resizing strip
		//////////////////////////////////////////////////

		sf::Vector2f bottomRight{m_shadedRectangle.getGlobalBounds().width,
								 m_shadedRectangle.getGlobalBounds().height};


		m_resizeStrip[0].position = bottomRight;
		m_resizeStrip[1].position = {bottomRight.x, bottomRight.y - RESIZESTRIP_HEIGHT};
		m_resizeStrip[2].position = {bottomRight.x - RESIZESTRIP_WIDTH, bottomRight.y};


		m_resizeStrip[0].color = {255, 255, 255, 140};
		m_resizeStrip[1].color = {0, 0, 0, 140};
		m_resizeStrip[2].color = {0, 0, 0, 140};

		//The remaining portion of this function isn't
		//needed unless the window was resized
		//////////////////////////////////////////////////
		if (!_resized) {
			return;
		}

		for (size_t i{0}; i < m_messages.size(); i++) {
			InfoBoxMessage& message = m_messages[i];
			positionMessage(i);
		}

		setupScrollbar();
	}

	////////////////////////////////////////////////////////////
	float InfoBox::getUsableWidth() const {
		return m_view.getSize().x - SCROLLBAR_WIDTH;
	}

	////////////////////////////////////////////////////////////
	float InfoBox::getMenuHeight() const {
		if (m_messages.empty()) {
			return 0;
		}

		auto firstTextPos{m_messages[0].getText().getPosition()};
		auto lastTextPos{m_messages.back().getText().getPosition()};

		auto firstTextBounds{m_messages[0].getText().getGlobalBounds()};
		auto lastTextBounds{m_messages.back().getText().getGlobalBounds()};

		//Since messages have a Y origin of 0, we'll add the last text's height

		return (lastTextPos.y + lastTextBounds.height) -
		       firstTextPos.y;
	}


	////////////////////////////////////////////////////////////
	void InfoBox::setupScrollbar() {
		if (m_messages.empty()) {
			m_scrollbar.setActive(false);
			return;
		}

		if (getMenuHeight() < m_size.y) {
			m_scrollbar.setActive(false);
			return;
		}

		m_scrollbar.setActive(true);

		//visibleHeight is essentially the height of the menu. However,
		//trueHeight will be used for all scrollbar size related
		//calculations.
		//
		//We'll subtract 15 and work under the assumption that the
		//scrollbar won't take up all vertical space; there will be
		//a little extra space at the bottom to be used for resizing.
		float visibleHeight{m_size.y};
		float trueHeight{visibleHeight - RESIZESTRIP_HEIGHT};

		float scrollbarX{m_shadedRectangleView.getCenter().x};
		scrollbarX += m_shadedRectangleView.getSize().x / 2;
		scrollbarX -= SCROLLBAR_WIDTH;

		sf::Vector2f scrollbarSize{SCROLLBAR_WIDTH, trueHeight};
		sf::Vector2f scrollbarPosition{scrollbarX, 0};

		//Adjust the ranges so that they don't leave excess empty space.
		//We'll add a little extra space (linespacing / 2) at the end to
		//ensure no text is cut off.
		float minRange{m_size.y / 2};
		float maxRange{getMenuHeight() - (visibleHeight / 2) + m_font.getLineSpacing(m_charSize) / 2};

		m_scrollbar.reset(scrollbarSize, scrollbarPosition, minRange, maxRange);
	}

	////////////////////////////////////////////////////////////
	void InfoBox::dragBox() {
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

		reset(false);
	}

	////////////////////////////////////////////////////////////
	void InfoBox::resizeBox() {
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

	////////////////////////////////////////////////////////////
	void InfoBox::detectBoxInteractions(sf::Event& _event) {
		if (!m_draggable) {
			return;
		}

		if (Util::Input::lmbPressed(_event)) {
			sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);

			if (boxMousedOver()) {
				//boxMousedOver() returns whether the mouse is anywhere
				//in the box - including the scrollbar and resizing strip.
				//We need to update m_lastMousePosition if LMB was clicked
				//anywhere within the box; however, we'll only set m_dragging
				//to true if the area that was clicked was in the box only,
				//excluding the scrollbar/resizing strip
				m_lastMousePosition = mousePos;

				auto pixelPos{m_window->mapPixelToCoords(mousePos, m_shadedRectangleView)};
				auto bounds{m_shadedRectangle.getGlobalBounds()};
				bounds.width -= SCROLLBAR_WIDTH;

				if (bounds.contains(pixelPos.x, pixelPos.y)) {
					m_dragging = true;
				}
			}
		}

		else if (Util::Input::lmbReleased(_event)) {
			m_dragging = false;
		}
	}

	////////////////////////////////////////////////////////////
	void  InfoBox::detectResizeStripInteractions(sf::Event& _event) {
		if (!m_resizable) {
			return;
		}

		if (Util::Input::lmbPressed(_event) && resizeStripMousedOver()) {
			m_resizing = true;
		}
		else if (Util::Input::lmbReleased(_event)) {
			m_resizing = false;
		}
	}

} // namespace spss
