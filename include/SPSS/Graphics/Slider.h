#ifndef SLIDER_H_INCLUDED
#define SLIDER_H_INCLUDED

////////////////////////////////////////////////////////////////////
///add comments
////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SPSS/Util/Input.h>

namespace spss {

	template<class ValueType>
	class Slider : public sf::Drawable {
	  public:
		////////////////////////////////////////////////////////////
		/// \brief Construct the Slider
		///
		///
		////////////////////////////////////////////////////////////
		Slider(float               _width,
			   const sf::Vector2f& _pos,
			   const std::string& _titleStr,
			   const sf::Font&    _font) :
				   m_color{sf::Color::White},
				   m_window{nullptr},
				   m_dragging{false},
				   m_titleStr{_titleStr},
				   m_title{_titleStr, _font, 17},
				   m_outer{},
				   m_inner{},
				   m_values{},
				   m_selectedIndex{0} {

			auto textBounds{m_title.getGlobalBounds()};
			m_title.setOrigin(textBounds.left, textBounds.top);

			m_outer.setOutlineThickness(-1);
			m_outer.setFillColor(sf::Color::Transparent);

			setWidth(_width);
			setPosition(_pos);
			setColor(m_color);
		};

		////////////////////////////////////////////////////////////
		/// \brief Get input
		///
		/// \param _event       A reference to a captured event
		///
		////////////////////////////////////////////////////////////
		inline void getInput(sf::Event& _event) {
			if (Util::Input::lmbPressed(_event) && mousedOver()) {
				m_dragging = true;
			}
			else if (Util::Input::lmbReleased(_event)) {
				m_dragging = false;
			}
		};

		////////////////////////////////////////////////////////////
		/// \brief Update
		///
		////////////////////////////////////////////////////////////
		inline void update() {
			drag();
		}

		////////////////////////////////////////////////////////////
		/// \brief Draw the text to a render window
		///
		/// \param target Render window to draw to
		/// \param states Current render states
		///
		////////////////////////////////////////////////////////////
		inline void draw(sf::RenderWindow& window, sf::RenderStates states) const {
			m_window = &window;
			m_window->draw(m_outer, states);
			m_window->draw(m_inner, states);
			m_window->draw(m_title, states);
		}

		////////////////////////////////////////////////////////////
		/// \brief Draw the text to a render target
		///
		/// Since we only want Slider to be drawable to sf::Render-
		/// Windows, and since sf::Drawable requires this function to
		/// be overloaded, we'll dynamically cast target to a sf::Render-
		/// Window. If that's not possible, then draw() will do nothing.
		///
		/// \param target Render target to draw to
		/// \param states Current render states
		///
		////////////////////////////////////////////////////////////
		inline void draw(sf::RenderTarget& target, sf::RenderStates states) const {
			sf::RenderWindow* w{dynamic_cast<sf::RenderWindow*>(&target)};
			if (w == nullptr) {
				return;
			}

			draw(*w, states);
		};

		////////////////////////////////////////////////////////////
		/// \brief Set the slider's width
		///
		/// \param _width The width
		///
		////////////////////////////////////////////////////////////
		inline void setWidth(const float _width) {
			m_outer.setSize({_width, 25});
			m_inner.setSize({_width, 25});
		};

		////////////////////////////////////////////////////////////
		/// \brief Set the slider's position
		///
		/// \param _pos The position
		///
		////////////////////////////////////////////////////////////
		inline void setPosition(const sf::Vector2f& _pos) {
			m_title.setPosition(_pos);
			m_inner.setPosition(_pos.x, _pos.y + 25);
			m_outer.setPosition(_pos.x, _pos.y + 25);

		};

		////////////////////////////////////////////////////////////
		/// \brief Set the slider's color
		///
		/// \param _c The color
		///
		////////////////////////////////////////////////////////////
		inline void setColor(const sf::Color& _c) {
			m_outer.setOutlineColor(_c);
			m_inner.setFillColor(_c);
			m_title.setFillColor(_c);
		};

		////////////////////////////////////////////////////////////
		/// \brief Get the slider's color
		///
		////////////////////////////////////////////////////////////
		inline const sf::Color& getColor() const {
			return m_inner.getFillColor();
		};

		//Add comments
		inline void addValue(const ValueType& _val,
							 const std::string& _str,
							 bool _select = false) {
			m_values.push_back({_val, _str});
			if (_select) {
				m_selectedIndex = m_values.size() - 1;
			}

			const float width{m_outer.getSize().x};

			sf::Vector2f newSize{m_inner.getSize()};
			newSize.x = width / m_values.size();
			m_inner.setSize(newSize);

			updateString();
		}

		//Add comments
		inline const ValueType* getSelectedValue() const {
			if (m_values.empty()) {
				return nullptr;
			}

			return &m_values[m_selectedIndex].m_value;
		}


	  private:
		////////////////////////////////////////////////////////////
		/// \brief Is the slider being moused over?
		///
		////////////////////////////////////////////////////////////
		inline bool mousedOver() const {
			if (m_window == nullptr) {
				return false;
			}

			auto& backgroundView{m_window->getView()};

			sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
			sf::Vector2f pixelPos{m_window->mapPixelToCoords(mousePos, backgroundView)};

			if (m_outer.getGlobalBounds().contains(pixelPos.x, pixelPos.y)) {
				return true;
			}

			return false;
		}

		////////////////////////////////////////////////////////////
		/// \brief Move the slider if it's being dragged
		///
		////////////////////////////////////////////////////////////
		inline void drag() {
			if (m_window == nullptr || !m_dragging) {
				return;
			}

			auto& backgroundView{m_window->getView()};

			auto pos{sf::Mouse::getPosition(*m_window)};
			auto mousePos{m_window->mapPixelToCoords(pos, backgroundView)};

			//Just some aliases for readability
			auto outerPos{m_outer.getPosition()};
			auto outerBounds{m_outer.getGlobalBounds()};
			auto innerPos{m_inner.getPosition()};
			auto innerBounds{m_inner.getGlobalBounds()};

			float lowerLimit{outerPos.x};
			float upperLimit{outerPos.x + outerBounds.width - innerBounds.width};

			innerPos.x = mousePos.x;

			if (innerPos.x <= lowerLimit) {
				innerPos.x = lowerLimit;
			}
			else if (innerPos.x >= upperLimit) {
				innerPos.x = upperLimit;
			}

			//Now that we have the slider's "true" position, we'll compare it
			//to the number of steps (i.e. elements in m_values) and snap it
			//into the appropriate position.

			const float width{m_outer.getSize().x};

			int vectorSize{1};
			if(m_values.size() > 1) {
				vectorSize = int(m_values.size());
			}

			int steps{int(width) / vectorSize};
			int index{(int(innerPos.x) - int(outerPos.x)) / steps};

			m_selectedIndex = size_t(index);
			innerPos.x = outerPos.x + (steps * index);
			m_inner.setPosition(innerPos);

			updateString();
		}

		//Add comments
		inline void updateString() {
			if(m_values.size() >= 1) {
				m_title.setString(m_titleStr + " " + m_values[m_selectedIndex].m_str);
			}
		}

		//Add comments
		struct SliderPair {
			SliderPair(const ValueType& _v, const std::string& _str) :
				m_value{_v}, m_str{_str} {};

			ValueType    m_value;
			std::string  m_str;
		};

		///////////////////////////////////////////////////////////
		//Data members --------------------------------------------
		///////////////////////////////////////////////////////////
		mutable sf::RenderWindow* m_window;          ///< The window to draw the slider in
		sf::Color                 m_color;           ///< The color of the slider
		mutable bool              m_dragging;        ///< Is the slider being dragged?
		std::string               m_titleStr;        ///< The original slider title text string
		sf::Text                  m_title;           ///< The slider's title text
		sf::RectangleShape        m_outer;           ///< The outer part of the slider
		sf::RectangleShape        m_inner;           ///< The inner part of the slider
		size_t                    m_selectedIndex;   ///< The index (of m_values) that's currently selected
		std::vector<SliderPair>   m_values;          ///< The vector that stores all the possible values and their display texts
	};

} // namespace spss

#endif // SLIDER_H_INCLUDED
