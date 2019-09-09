#include <SPSS/System/MenuState.h>

namespace spss {

	////////////////////////////////////////////////////////////
	MenuState::MenuState(sf::RenderWindow&  _w,
	                     const sf::Font&    _f,
	                     const std::string& _titleStr)

	            : State{_w},
	              m_font{_f},
	              m_view{sf::FloatRect(0,
	                                   0,
	                                   float(m_window.getSize().x),
	                                   float(m_window.getSize().y))},
	              m_backgroundView{m_view},
	              m_titleText{},
	              m_randomiseTextColor{false},
	              m_randomColorDurationMS{500},
	              m_scaleText{false},
	              m_minScale{0.75},
	              m_maxScale{1.25},
	              m_rotateText{false},
	              m_rotationDegrees{10},
	              m_highlightColor{sf::Color::Yellow},
	              m_highlightScale{1.0F},
	              m_scrollbarColor{sf::Color::White},
	              m_scrollbarActive{false},
	              m_scrollbarDragging{false},
	              m_scrollbarMinRange{0},
	              m_scrollbarMaxRange{0} {
		m_window.setView(m_view);
		m_titleText.setFont(m_font);
		m_titleText.setCharacterSize(64);
		m_titleText.setString(_titleStr);
		m_titleText.setOutlineThickness(1);
		m_titleText.setOrigin(m_titleText.getLocalBounds().width / 2,
		                      m_titleText.getLocalBounds().height / 2);

		onResize(m_window.getSize());
	}

	////////////////////////////////////////////////////////////
	void MenuState::getInput(sf::Event& _event) {
		State::getInput(_event);

		detectScrollbarInteractions(_event);

		if (lmbPressed(_event)) {
			for (const auto& menuItem : m_menuItems) {
				if (isMousedOver(menuItem) && !isFunctionNull(menuItem)) {
					menuItem.boundFunction(std::any());
					return;
				}
			}
		}

		if (_event.type == sf::Event::KeyPressed) {
			for (const auto& menuItem : m_menuItems) {
				if (menuItem.keyCode == sf::Keyboard::Unknown) {
					continue;
				}

				if (_event.key.code == menuItem.keyCode && !isFunctionNull(menuItem)) {
					menuItem.boundFunction(std::any());
					return;
				}
			}
		}
	}

	////////////////////////////////////////////////////////////
	void MenuState::update(int _timeslice) {
		detectMouseClicks();
		updateTitleText(_timeslice);
		updateScrollbar();
	}

	////////////////////////////////////////////////////////////
	void MenuState::draw() {
		m_window.setView(m_view);
		for (const auto& menuItem : m_menuItems) {
			m_window.draw(menuItem.text);
		}

		m_window.setView(m_backgroundView);

		if (m_scrollbarActive) {
			m_window.draw(m_scrollbarOuter);
			m_window.draw(m_scrollbarInner);
		}

		m_window.draw(m_titleText);
	}

	////////////////////////////////////////////////////////////
	void MenuState::addMenuItem(const std::string&       _string,
	                            spss::Function<std::any> f,
	                            int                      _keyCode) {
		sf::Text text;

		text.setFont(m_font);
		text.setOutlineThickness(1);
		text.setCharacterSize(34);
		text.setString(_string);
		text.setOrigin(text.getLocalBounds().width / 2, 0);

		m_menuItems.push_back({false, f, text, _keyCode});

		onResize(m_window.getSize());
	}

	////////////////////////////////////////////////////////////
	void MenuState::addGap() {
		addMenuItem("");
	}

	////////////////////////////////////////////////////////////
	void MenuState::setScrollbarColor(sf::Color _c) {
		m_scrollbarColor = _c;
	}

	////////////////////////////////////////////////////////////
	void MenuState::resetViews(sf::Vector2u _newSize) {
		m_view.reset(
		  {0, 0, float(_newSize.x), float(_newSize.y)});
		m_backgroundView.reset(
		  {0, 0, float(_newSize.x), float(_newSize.y)});
	}

	////////////////////////////////////////////////////////////
	void MenuState::adjustViewport() {
		float titleY{m_view.getSize().y / 10};
		float top{titleY + m_titleText.getLocalBounds().height};
		m_view.setViewport({0.F, top / m_view.getSize().y, 1.F, 1.F});
	}

	////////////////////////////////////////////////////////////
	void MenuState::resetTitlePosition() {
		m_titleText.setPosition({m_view.getCenter().x,
		                         m_titleText.getLocalBounds().height});
	}

	////////////////////////////////////////////////////////////
	void MenuState::adjustMenuItems() {
		if (m_menuItems.empty()) {
			return;
		}

		sf::Vector2f pos{m_view.getCenter().x, 0.F};

		auto it = m_menuItems.begin();
		while (it != m_menuItems.end()) {
			(*it).text.setPosition(pos);
			pos.y += 1.25F * m_font.getLineSpacing((*it).text.getCharacterSize());
			++it;
		}
	}

	////////////////////////////////////////////////////////////
	void MenuState::adjustBoundaries() {
		if (m_menuItems.empty()) {
			return;
		}

		float upper{m_titleText.getPosition().y - m_titleText.getLocalBounds().height * 2};
		float lower{m_view.getSize().y -
		            (m_menuItems.back()).text.getPosition().y};

		lower -= upper;
		lower -= 1.25F * m_font.getLineSpacing(m_titleText.getCharacterSize());
		lower -= m_titleText.getLocalBounds().height * 2;

		float diff{float(abs(upper - lower))};

		if (diff == 0) {
			return;
		}

		float halfDiff{diff / 2};

		if (upper > lower) {
			upper -= halfDiff;
			lower += halfDiff;
		}
		else if (lower > upper) {
			upper += halfDiff;
			lower -= halfDiff;
		}

		if (upper < m_titleText.getLocalBounds().height / 2) {
			upper = m_titleText.getLocalBounds().height / 2;
		}

		auto titlePos{m_titleText.getPosition()};
		titlePos.y = upper;
		m_titleText.setPosition(titlePos);

		upper += m_titleText.getLocalBounds().height;
		upper += 1.25F * m_font.getLineSpacing(m_titleText.getCharacterSize());

		m_view.setViewport({0.F, (upper / m_view.getSize().y), 1.F, 1.F});
	}

	////////////////////////////////////////////////////////////
	void MenuState::detectMouseClicks() {
		for (auto& menuItem : m_menuItems) {
			//////////////////////////////////////////////////////////////
			/// In order to avoid the menu items being rapidly detected
			/// as moused over and then not moused over when the mouse is
			/// near the edge, we'll set the scale to the default level
			/// when detecting if it's moused over.
			//////////////////////////////////////////////////////////////
			menuItem.text.setScale({1.0F, 1.0F});

			if (isMousedOver(menuItem) && !isFunctionNull(menuItem)) {
				menuItem.mousedOver = true;
				menuItem.text.setFillColor(m_highlightColor);
				menuItem.text.setScale({m_highlightScale, m_highlightScale});
			}
			else {
				menuItem.mousedOver = false;
				if (menuItem.boundFunction == nullptr) {
					//reduce the opacity of items that don't do anything
					//when clicked on
					sf::Color cFill{menuItem.text.getFillColor()};
					cFill.a = 100;
					menuItem.text.setFillColor(cFill);

					sf::Color cOut{menuItem.text.getOutlineColor()};
					cOut.a = 100;
					menuItem.text.setOutlineColor(cOut);
				}
				else {
					menuItem.text.setFillColor(sf::Color::White);
				}
				menuItem.text.setScale({1.0F, 1.0F});
			}
		}
	}

	////////////////////////////////////////////////////////////
	void MenuState::updateTitleText(int _timeslice) {
		float f_timeslice{float(_timeslice)};
		//-------------------------------------------------
		//randomize fill color every half second
		if (m_randomiseTextColor) {
			static sf::Clock clock;

			if (clock.getElapsedTime().asMilliseconds() >= m_randomColorDurationMS) {
				auto                               seed{std::chrono::high_resolution_clock::now().time_since_epoch().count()};
				static std::mt19937                mt_rand(seed);
				std::uniform_int_distribution<int> uniform_dist(0, 255);
				int                                r{uniform_dist(mt_rand)};
				int                                g{uniform_dist(mt_rand)};
				int                                b{uniform_dist(mt_rand)};

				int rx{uniform_dist(mt_rand)};
				int gx{uniform_dist(mt_rand)};
				int bx{uniform_dist(mt_rand)};

				m_titleText.setFillColor(sf::Color(r, g, b));
				m_titleText.setOutlineColor(sf::Color(rx, gx, bx));

				clock.restart();
			}
		}
		//-------------------------------------------------
		//Rotate the title text
		if (m_rotateText) {
			static bool rotatingLeft{false};
			float       deltaRotation{0};
			const float curRotation{m_titleText.getRotation()};

			if (Util::Math::almostEqual(curRotation, m_rotationDegrees)) {
				rotatingLeft = true;
			}
			else if (Util::Math::almostEqual(curRotation, (360 - m_rotationDegrees))) {
				rotatingLeft = false;
			}

			if (rotatingLeft) {
				deltaRotation = -1 * (f_timeslice / 1000.F);
			}
			else {
				deltaRotation = (f_timeslice / 1000.F);
			}
			m_titleText.rotate(deltaRotation);
		}

		//-------------------------------------------------
		//Scale the title text
		if (m_scaleText) {
			static sf::Vector2f minScale{m_minScale, m_minScale};
			static sf::Vector2f maxScale{m_maxScale, m_maxScale};
			static bool         scalingDown{false};
			static sf::Vector2f deltaScale{1, 1};

			if (maxScale.x <= deltaScale.x) {
				scalingDown = true;
			}
			else if (minScale.x >= deltaScale.x) {
				scalingDown = false;
			}

			if (scalingDown) {
				deltaScale.x -= f_timeslice / 10000.F;
				deltaScale.y -= f_timeslice / 10000.F;
			}
			else {
				deltaScale.x += f_timeslice / 10000.F;
				deltaScale.y += f_timeslice / 10000.F;
			}
			m_titleText.setScale(deltaScale);
		}
	}

	////////////////////////////////////////////////////////////
	bool MenuState::isMousedOver(const MenuItem& _menuItem) const {
		sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
		sf::Vector2f pixelPos{m_window.mapPixelToCoords(mousePos, m_view)};
		return _menuItem.text.getGlobalBounds().contains(pixelPos);
	}

	////////////////////////////////////////////////////////////
	bool MenuState::isFunctionNull(const MenuItem& _menuItem) const {
		return _menuItem.boundFunction == nullptr;
	}

	////////////////////////////////////////////////////////////
	bool MenuState::lmbPressed(sf::Event& _event) {
		if (_event.type == sf::Event::MouseButtonPressed &&
		    _event.mouseButton.button == sf::Mouse::Left) {
			return true;
		}
		else {
			return false;
		}
	}

	////////////////////////////////////////////////////////////
	bool MenuState::lmbReleased(sf::Event& _event) {
		if (_event.type == sf::Event::MouseButtonReleased &&
		    _event.mouseButton.button == sf::Mouse::Left) {
			return true;
		}
		else {
			return false;
		}
	}

	////////////////////////////////////////////////////////////
	void MenuState::onResize(sf::Vector2u _newSize) {
		State::onResize(_newSize);
		resetViews(_newSize);
		adjustViewport();
		resetTitlePosition();
		adjustMenuItems();
		adjustBoundaries();
		adjustScrollbar();
	}

	////////////////////////////////////////////////////////////
	void MenuState::setRandomisedColors(bool _b, int _duration) {
		m_randomiseTextColor    = _b;
		m_randomColorDurationMS = _duration;
	}

	////////////////////////////////////////////////////////////
	void MenuState::setScalingText(bool _b, float _minScale, float _maxScale) {
		m_scaleText = _b;
		if (_minScale > 0 && _maxScale > _minScale) {
			m_minScale = _minScale;
			m_maxScale = _maxScale;
		}
	}

	////////////////////////////////////////////////////////////
	void MenuState::setRotatingText(bool _b, float _rotationDegrees) {
		m_rotateText = _b;
		if (_rotationDegrees > 0) {
			m_rotationDegrees = _rotationDegrees;
		}
	}

	////////////////////////////////////////////////////////////
	void MenuState::setTitleString(const std::string& _str) {
		m_titleText.setString(_str);
	}

	////////////////////////////////////////////////////////////
	void MenuState::setTitleSize(unsigned int _size) {
		m_titleText.setCharacterSize(_size);
	}

	////////////////////////////////////////////////////////////
	void MenuState::setTitleColor(sf::Color _color) {
		setRandomisedColors(false);
		m_titleText.setFillColor(_color);
	}

	////////////////////////////////////////////////////////////
	void MenuState::setMenuItemsSize(unsigned int _size) {
		for (auto& item : m_menuItems) {
			item.text.setCharacterSize(_size);
		}
	}

	////////////////////////////////////////////////////////////
	void MenuState::setMenuItemsColor(sf::Color _color) {
		for (auto& item : m_menuItems) {
			item.text.setFillColor(_color);
		}
	}

	////////////////////////////////////////////////////////////
	void MenuState::setHighlightColor(sf::Color _color) {
		m_highlightColor = _color;
	}

	////////////////////////////////////////////////////////////
	void MenuState::setHighlightScale(float _f) {
		if (_f > 0) {
			m_highlightScale = _f;
		}
	}

	////////////////////////////////////////////////////////////
	float MenuState::getMenuOffset() const {
		return (m_view.getViewport().top * m_window.getSize().y);
	}

	////////////////////////////////////////////////////////////
	float MenuState::getMenuHeight() const {
		if (m_menuItems.empty()) {
			return 0;
		}

		const sf::Text& firstText{m_menuItems[0].text};
		const sf::Text& lastText{m_menuItems.back().text};

		//Since menuItems have a Y origin of 0, we'll add the last text's height

		return (lastText.getPosition().y + lastText.getGlobalBounds().height) -
		       firstText.getPosition().y;
	}

	////////////////////////////////////////////////////////////
	void MenuState::updateScrollbar() {
		if (!m_scrollbarActive) {
			return;
		}
		dragScrollbar();
		calculateNewScrollbarCenter();
	}
	void MenuState::adjustScrollbar() {
		if (m_menuItems.empty()) {
			return;
		}

		if (getMenuHeight() + getMenuOffset() < m_window.getSize().y) {
			m_scrollbarActive = false;
			return;
		}

		m_scrollbarActive = true;

		float visibleHeight{m_window.getSize().y - getMenuOffset()};

		//We'll find the widest text object so that we can position the
		//scrollbar as close as possible without being obtrusive
		sf::Text& widestText{m_menuItems[0].text};
		for (auto& item : m_menuItems) {
			if (item.text.getGlobalBounds().width > widestText.getGlobalBounds().width) {
				widestText = item.text;
			}
		}

		//Adjust the ranges so that they don't leave excess empty space
		m_scrollbarMinRange = m_window.getSize().y / 2;
		m_scrollbarMaxRange = getMenuHeight() + getMenuOffset() - (visibleHeight / 2);

		//Initialize the outer scrollbar
		m_scrollbarOuter.setFillColor(sf::Color::Transparent);
		m_scrollbarOuter.setOutlineColor(m_scrollbarColor);
		m_scrollbarOuter.setOutlineThickness(1);
		m_scrollbarOuter.setPosition({20 + widestText.getPosition().x + widestText.getGlobalBounds().width / 2,
		                              getMenuOffset() - 10});
		m_scrollbarOuter.setSize({15, visibleHeight});

		//Initialize the inner scrollbar

		//The ratio of inner height  :  outer height (i.e. visibleHeight)
		//should equal
		//the ratio of visibleHeight :  total menu height
		//so that the inner scrollbar's height scales linearly with the
		//amount of visible content
		float ratio{visibleHeight / getMenuHeight()};
		m_scrollbarInner.setFillColor(m_scrollbarColor);
		m_scrollbarInner.setSize({15, visibleHeight * ratio});
		m_scrollbarInner.setPosition({m_scrollbarOuter.getPosition()});
	}

	void MenuState::dragScrollbar() {
		if (!m_scrollbarDragging) {
			return;
		}

		auto mousePos{sf::Mouse::getPosition(m_window)};

		//Just some aliases for readability
		auto outerPos{m_scrollbarOuter.getPosition()};
		auto outerBounds{m_scrollbarOuter.getGlobalBounds()};
		auto innerPos{m_scrollbarInner.getPosition()};
		auto innerBounds{m_scrollbarInner.getGlobalBounds()};

		float lowerLimit{outerPos.y};
		float upperLimit{outerPos.y + outerBounds.height - innerBounds.height};
		upperLimit -= 2 * m_scrollbarOuter.getOutlineThickness();

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

	void MenuState::detectScrollbarInteractions(sf::Event& _event) {
		if (lmbPressed(_event)) {
			auto mousePos{sf::Mouse::getPosition(m_window)};
			if (m_scrollbarOuter.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
				m_scrollbarDragging = true;
			}
		}
		else if (lmbReleased(_event)) {
			m_scrollbarDragging = false;
		}
		else if (_event.type == sf::Event::MouseWheelMoved) {
			if (_event.mouseWheel.delta > 0) {
				scroll(true);
			}
			else if (_event.mouseWheel.delta < 0) {
				scroll(false);
			}
		}
	}

	void MenuState::calculateNewScrollbarCenter() {
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

	void MenuState::scroll(bool _up) {
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
} // namespace spss
