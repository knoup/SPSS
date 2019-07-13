#include <SPSS/System/MenuState.h>

namespace spss {

    MenuState::MenuState(sf::RenderWindow&  _w,
                         const sf::Font&              _f,
                         const std::string&           _titleStr)

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
          m_highlightScale{1.0F} {

        m_window.setView(m_view);
        m_titleText.setFont(m_font);
        m_titleText.setCharacterSize(64);
        m_titleText.setString(_titleStr);
        m_titleText.setOutlineThickness(1);
        m_titleText.setOrigin(m_titleText.getLocalBounds().width / 2,
                              m_titleText.getLocalBounds().height / 2);

        onResize(m_window.getSize());
    }

    void MenuState::getInput(sf::Event& _event) {
        State::getInput(_event);

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

    void MenuState::update(int _timeslice) {
        detectMouseClicks();
        updateTitleText(_timeslice);
    }

    void MenuState::draw() {
        m_window.setView(m_view);
        for (const auto& menuItem : m_menuItems) {
            m_window.draw(menuItem.text);
        }

        m_window.setView(m_backgroundView);
        m_window.draw(m_titleText);
    }

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


        resetTitlePosition();
        adjustMenuItems();
        adjustBoundaries();
    }

    void MenuState::addGap() {
        addMenuItem("");
    }


    void MenuState::resetViews(sf::Vector2u _newSize) {
        m_view.reset(
        {0, 0, float(_newSize.x), float(_newSize.y)});
        m_backgroundView.reset(
        {0, 0, float(_newSize.x), float(_newSize.y)});
    }

    void MenuState::adjustViewport() {
        float titleY{m_view.getSize().y / 10};
        float top{titleY + m_titleText.getLocalBounds().height};
        m_view.setViewport({0.F, top / m_view.getSize().y, 1.F, 1.F});
    }

    void MenuState::resetTitlePosition() {
        m_titleText.setPosition({m_view.getCenter().x,
                                 m_titleText.getLocalBounds().height});
    }

    void MenuState::adjustMenuItems() {
        if(m_menuItems.empty()) {
            return;
        }

        sf::Vector2f pos{m_view.getCenter().x,0.F};

        auto it = m_menuItems.begin();
        while (it != m_menuItems.end()) {
            (*it).text.setPosition(pos);
            pos.y += 1.25F * m_font.getLineSpacing((*it).text.getCharacterSize());
            ++it;
        }
    }

    void MenuState::adjustBoundaries() {
        if(m_menuItems.empty()) {
            return;
        }

        float upper{m_titleText.getPosition().y - m_titleText.getLocalBounds().height * 2};
        float lower{m_view.getSize().y
                    -
                    (m_menuItems.back()).text.getPosition().y};

        lower -= upper;
        lower -= 1.25F * m_font.getLineSpacing(m_titleText.getCharacterSize());
        lower -= m_titleText.getLocalBounds().height * 2;

        float diff{float(abs(upper-lower))};

        if(diff == 0) {
            return;
        }

        float halfDiff{diff / 2};

        if(upper > lower) {
            upper -= halfDiff;
            lower += halfDiff;
        }
        else if (lower > upper) {
            upper += halfDiff;
            lower -= halfDiff;
        }

        if(upper < m_titleText.getLocalBounds().height / 2) {
            upper = m_titleText.getLocalBounds().height / 2;
        }

        auto titlePos{m_titleText.getPosition()};
        titlePos.y = upper;
        m_titleText.setPosition(titlePos);

        upper += m_titleText.getLocalBounds().height;
        upper += 1.25F * m_font.getLineSpacing(m_titleText.getCharacterSize());

        m_view.setViewport({0.F, (upper / m_view.getSize().y), 1.F, 1.F});
    }

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

    void MenuState::updateTitleText(int _timeslice) {
        float f_timeslice{float(_timeslice)};
        //-------------------------------------------------
        //randomize fill color every half second
        if(m_randomiseTextColor) {
            static sf::Clock clock;

            if (clock.getElapsedTime().asMilliseconds() >= m_randomColorDurationMS) {
                auto seed =
                    std::chrono::high_resolution_clock::now().time_since_epoch().count();
                static std::mt19937                mt_rand(seed);
                std::uniform_int_distribution<int> uniform_dist(0, 255);
                int                                r = uniform_dist(mt_rand);
                int                                g = uniform_dist(mt_rand);
                int                                b = uniform_dist(mt_rand);

                int rx = uniform_dist(mt_rand);
                int gx = uniform_dist(mt_rand);
                int bx = uniform_dist(mt_rand);

                m_titleText.setFillColor(sf::Color(r, g, b));
                m_titleText.setOutlineColor(sf::Color(rx, gx, bx));

                clock.restart();
            }
        }
        //-------------------------------------------------
        //Rotate the title text
        if(m_rotateText) {
            static bool  rotatingLeft{false};
            float        deltaRotation{0};
            const float  curRotation{m_titleText.getRotation()};

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
        if(m_scaleText) {
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

    bool MenuState::isMousedOver(const MenuItem& _menuItem) const {
        sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
        sf::Vector2f pixelPos{m_window.mapPixelToCoords(mousePos, m_view)};
        return _menuItem.text.getGlobalBounds().contains(pixelPos);
    }

    bool MenuState::isFunctionNull(const MenuItem& _menuItem) const {
        return _menuItem.boundFunction == nullptr;
    }

    bool MenuState::lmbPressed(sf::Event& _event) {
        if (_event.type == sf::Event::MouseButtonPressed &&
                _event.mouseButton.button == sf::Mouse::Left) {
            return true;
        }
        else {
            return false;
        }
    }

    void MenuState::onResize(sf::Vector2u _newSize) {
        State::onResize(_newSize);
        resetViews(_newSize);
        adjustViewport();
        resetTitlePosition();
        adjustMenuItems();
        adjustBoundaries();
    }

    void MenuState::setRandomisedColors(bool _b, int _duration) {
        m_randomiseTextColor = _b;
        m_randomColorDurationMS = _duration;
    }

    void MenuState::setScalingText(bool _b, float _minScale, float _maxScale) {
        m_scaleText = _b;
        if(_minScale > 0 && _maxScale > _minScale) {
            m_minScale = _minScale;
            m_maxScale = _maxScale;
        }
    }

    void MenuState::setRotatingText(bool _b, float _rotationDegrees) {
        m_rotateText = _b;
        if(_rotationDegrees > 0) {
            m_rotationDegrees = _rotationDegrees;
        }
    }

    void MenuState::setTitleString(const std::string& _str) {
        m_titleText.setString(_str);
    }

    void MenuState::setTitleSize(unsigned int _size) {
        m_titleText.setCharacterSize(_size);
    }

    void MenuState::setTitleColor(sf::Color _color) {
        setRandomisedColors(false);
        m_titleText.setFillColor(_color);
    }

    void MenuState::setMenuItemsSize(unsigned int _size) {
        for(auto& item : m_menuItems) {
            item.text.setCharacterSize(_size);
        }
    }

    void MenuState::setMenuItemsColor(sf::Color _color) {
        for(auto& item : m_menuItems) {
            item.text.setFillColor(_color);
        }
    }

    void MenuState::setHighlightColor(sf::Color _color) {
        m_highlightColor = _color;
    }

    void MenuState::setHighlightScale(float _f) {
        if(_f > 0) {
            m_highlightScale = _f;
        }
    }

} //namespace spss
