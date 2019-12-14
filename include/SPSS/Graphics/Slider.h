#ifndef SLIDER_H_INCLUDED
#define SLIDER_H_INCLUDED

////////////////////////////////////////////////////////////////////
/// spss::Slider can be used to present the user with multiple sele-
/// ctable values. The slider's inner bar will automatically adjust
/// its size depending on the number of options, and by dragging the
/// inner part of the slider, users can change the currently select-
/// ed option.
///
/// Slider can be used with any type. Each of the possible selectable
/// values has a string associated with it so that the user can see
/// what they're selecting (see SliderPair further below).
///
////////////////////////////////////////////////////////////////////

#include <SPSS/Graphics/DrawableToWindow.h>
#include <SFML/Graphics.hpp>
#include <SPSS/Util/Input.h>

namespace spss {

	template<class ValueType>
	class Slider : public spss::DrawableToWindow {
	  public:
		////////////////////////////////////////////////////////////
		/// \brief Construct the Slider
		///
		////////////////////////////////////////////////////////////
		Slider(const sf::Vector2f& _size,
		       const sf::Vector2f& _pos,
		       const std::string&  _titleStr,
		       const sf::Font&     _font,
		       const unsigned int  _fontSize = 17)
		            : m_color{sf::Color::White},
		              m_window{nullptr},
		              m_dragging{false},
		              m_titleStr{_titleStr},
		              m_title{_titleStr, _font, _fontSize},
		              m_outer{},
		              m_inner{},
		              m_options{},
		              m_selected{0} {
			auto textBounds{m_title.getGlobalBounds()};
			m_title.setOrigin(textBounds.left, textBounds.top);

			m_outer.setOutlineThickness(-1);
			m_outer.setFillColor(sf::Color::Transparent);

			setSize(_size);
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
		/// \brief Set the slider's size
		///
		/// \param _size The size
		///
		////////////////////////////////////////////////////////////
		inline void setSize(const sf::Vector2f& _size) {
			m_outer.setSize(_size);
			m_inner.setSize(_size);
		};

		////////////////////////////////////////////////////////////
		/// \brief Set the slider's position
		///
		/// \param _pos The position
		///
		////////////////////////////////////////////////////////////
		inline void setPosition(const sf::Vector2f& _pos) {
			float textHeight{m_title.getGlobalBounds().height};

			m_title.setPosition(_pos);
			m_inner.setPosition(_pos.x, _pos.y + textHeight + 5);
			m_outer.setPosition(_pos.x, _pos.y + textHeight + 5);

			select(m_selected);
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
		/// \brief Set the slider's origin
		///
		/// \param _o The origin
		///
		////////////////////////////////////////////////////////////
		inline void setOrigin(const sf::Vector2f& _o) {
			m_title.setOrigin(_o);
			m_outer.setOrigin(_o);
			m_inner.setOrigin(_o);
		}

		////////////////////////////////////////////////////////////
		/// \brief Get the slider's color
		///
		////////////////////////////////////////////////////////////
		inline const sf::Color& getColor() const {
			return m_inner.getFillColor();
		};

		////////////////////////////////////////////////////////////
		/// \brief Get the slider's size (the rectangle shape)
		///
		////////////////////////////////////////////////////////////
		inline const sf::Vector2f getSize() const {
			return m_outer.getSize();
		};

		////////////////////////////////////////////////////////////
		/// \brief Get the slider's position (the rectangle shape)
		///
		////////////////////////////////////////////////////////////
		inline const sf::Vector2f getPosition() const {
			return m_outer.getPosition();
		};

		////////////////////////////////////////////////////////////
		/// \brief Get the slider's local bounds (the rectangle shape)
		///
		////////////////////////////////////////////////////////////
		inline const sf::FloatRect getLocalBounds() const {
			return m_outer.getLocalBounds();
		};

		////////////////////////////////////////////////////////////
		/// \brief Get the slider's global bounds (the rectangle shape)
		///
		////////////////////////////////////////////////////////////
		inline const sf::FloatRect getGlobalBounds() const {
			return m_outer.getGlobalBounds();
		};

		////////////////////////////////////////////////////////////
		/// \brief Adds a selection option
		///
		/// \param _val    The value associated with this option
		/// \param _val    The string associated with this option
		/// \param _select Is this selected by default?
		///
		////////////////////////////////////////////////////////////
		inline void addValue(const ValueType&   _val,
		                     const std::string& _str,
		                     bool               _select = false) {
			m_options.push_back({_val, _str});

			//After adding a new value, the size of the inner bar
			//will have to be modified.
			const float width{m_outer.getSize().x};

			sf::Vector2f newSize{m_inner.getSize()};
			newSize.x = width / m_options.size();
			m_inner.setSize(newSize);

			//Say we call something like this:
			//addValue(1, "1");
			//addValue(2, "2", true);
			//addValue(3, "3");

			//When the second line is called and 2 is set as
			//the default selected value, by the time 3 is
			//added, the inner bar's size (and thus position)
			//will have changed. Therefore, we'll be sure to
			//call select() every time a new value is added,
			//to ensure that the bar's position is always
			//correct.

			if (_select) {
				select(m_options.size() - 1);
			}
			else {
				select(m_selected);
			}
		}

		////////////////////////////////////////////////////////////
		/// \brief Get the slider's selected value
		///
		////////////////////////////////////////////////////////////
		inline const ValueType* getSelected() const {
			if (m_options.empty()) {
				return nullptr;
			}

			return &m_options[m_selected].m_value;
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
			auto outerBounds{m_outer.getGlobalBounds()};

			auto innerPos{m_inner.getPosition()};
			auto innerBounds{m_inner.getGlobalBounds()};

			float lowerLimit{outerBounds.left};
			float upperLimit{outerBounds.left + outerBounds.width - innerBounds.width};

			innerPos.x = mousePos.x;

			if (innerPos.x <= lowerLimit) {
				innerPos.x = lowerLimit;
			}
			else if (innerPos.x >= upperLimit) {
				innerPos.x = upperLimit;
			}

			//Now that we have the slider's "true" position, we'll compare it
			//to the number of steps (i.e. elements in m_options) and snap it
			//into the appropriate position.

			int index{(int(innerPos.x) - int(lowerLimit)) / getStepValue()};

			select(size_t(index));
		}

		////////////////////////////////////////////////////////////
		/// \brief Get the step value
		///
		/// The step value is the outer bar's total width divided by
		/// the number of selection options we have. It represents
		/// the distance the inner bar travels from one selection
		/// to another. For example, if our slider has a width of 100
		/// and 2 selection options, this will return 50.
		///
		////////////////////////////////////////////////////////////
		inline int getStepValue() const {
			const float width{m_outer.getSize().x};

			//If there are no options, we'll return the width of the
			//slider to avoid dividing by zero
			int vectorSize{1};
			if (m_options.size() > 1) {
				vectorSize = int(m_options.size());
			}

			return {int(width) / vectorSize};
		}

		////////////////////////////////////////////////////////////
		/// \brief Selects the option at the given index
		///
		/// Handles not only snapping the inner bar into the approp-
		/// riate position, but also updating the title text.
		///
		/// \param _i The desired index to select
		///
		////////////////////////////////////////////////////////////
		inline void select(size_t _i) {
			m_selected = _i;

			auto outerPos{m_outer.getPosition()};
			auto innerPos{m_inner.getPosition()};

			innerPos.x = outerPos.x + (getStepValue() * int(m_selected));
			m_inner.setPosition(innerPos);

			if (m_options.size() >= 1) {
				m_title.setString(m_titleStr + " " + m_options[m_selected].m_str);
			}
		}

		//A simple struct we'll use to represent slider selection options.
		struct SliderPair {
			SliderPair(const ValueType& _v, const std::string& _str)
			            : m_value{_v}, m_str{_str} {};

			ValueType   m_value; ///< The value this option represents
			std::string m_str;   ///< The associated string
		};

		///////////////////////////////////////////////////////////
		//Data members --------------------------------------------
		///////////////////////////////////////////////////////////
		mutable sf::RenderWindow* m_window;   ///< The window to draw the slider in
		sf::Color                 m_color;    ///< The color of the slider
		mutable bool              m_dragging; ///< Is the slider being dragged?
		std::string               m_titleStr; ///< The original slider title text string
		sf::Text                  m_title;    ///< The slider's title text
		sf::RectangleShape        m_outer;    ///< The outer part of the slider
		sf::RectangleShape        m_inner;    ///< The inner part of the slider
		size_t                    m_selected; ///< The index (of m_options) that's currently selected
		std::vector<SliderPair>   m_options;  ///< The vector that stores all the SliderPairs
	};

} // namespace spss

#endif // SLIDER_H_INCLUDED
