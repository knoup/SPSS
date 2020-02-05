#include "SPSS/System/State.h"

constexpr float MIN_WIDTH{800};
constexpr float MIN_HEIGHT{600};

namespace spss {

	////////////////////////////////////////////////////////////
	State::State(sf::RenderWindow& _w)
	            : m_window{_w},
	              m_pollPreviousState{false},
	              m_drawPreviousState{false},
	              m_updatePreviousState{false} {
	}

	////////////////////////////////////////////////////////////
	State::~State() {
	}

	////////////////////////////////////////////////////////////
	void State::getInput(sf::Event& _event) {
		if (_event.type == sf::Event::Resized) {
			sf::Vector2u newSize{_event.size.width, _event.size.height};
			if (newSize.x < MIN_WIDTH) {
				newSize.x = MIN_WIDTH;
			}
			if (newSize.y < MIN_HEIGHT) {
				newSize.y = MIN_HEIGHT;
			}
			onResize(newSize);
		}
	}

	////////////////////////////////////////////////////////////
	void State::onStateSwitch() {
		onResize(m_window.getSize());
	}

	////////////////////////////////////////////////////////////
	void State::pollPreviousState(bool _b) {
		m_pollPreviousState = _b;
	}

	////////////////////////////////////////////////////////////
	void State::drawPreviousState(bool _b) {
		m_drawPreviousState = _b;
	}

	////////////////////////////////////////////////////////////
	void State::updatePreviousState(bool _b) {
		m_updatePreviousState = _b;
	}

	////////////////////////////////////////////////////////////
	bool State::previousStatePolled() const {
		return m_pollPreviousState;
	}

	////////////////////////////////////////////////////////////
	bool State::previousStateDrawn() const {
		return m_drawPreviousState;
	}

	////////////////////////////////////////////////////////////
	bool State::previousStateUpdated() const {
		return m_updatePreviousState;
	}

	////////////////////////////////////////////////////////////
	void State::onResize(sf::Vector2u _newSize) {
		m_window.setSize(_newSize);
	}

} //namespace spss
