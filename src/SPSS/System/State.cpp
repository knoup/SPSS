#include "SPSS/System/State.h"

constexpr float MIN_WIDTH{800};
constexpr float MIN_HEIGHT{600};

namespace spss {

    State::State(sf::RenderWindow& _w)
        : m_window{_w},
          m_drawnInBackground{false},
          m_updatedInBackground{false},
          m_getInputInBackground{false} {
    }

    State::~State() {
    }

    void State::getInput(sf::Event& _event) {
        switch (_event.type) {
            case sf::Event::Resized: {
                sf::Vector2u newSize{_event.size.width, _event.size.height};
                if (newSize.x < MIN_WIDTH) {
                    newSize.x = MIN_WIDTH;
                }
                if (newSize.y < MIN_HEIGHT) {
                    newSize.y = MIN_HEIGHT;
                }
                onResize(newSize);
                break;
            }

            default:
                break;

        }
    }

    void State::onStateSwitch() {
        onResize(m_window.getSize());
    }

    void State::setGetInputInBackground(bool _b) {
        m_getInputInBackground = _b;
    }

    void State::setDrawnInBackground(bool _b) {
        m_drawnInBackground = _b;
    }

    void State::setUpdatedInBackground(bool _b) {
        m_updatedInBackground = _b;
    }

    bool State::drawnInBackground() const {
        return m_drawnInBackground;
    }

    bool State::updatedInBackground() const {
        return m_updatedInBackground;
    }

    bool State::getInputInBackground() const {
        return m_getInputInBackground;
    }

    void State::onResize(sf::Vector2u _newSize) {
        m_window.setSize(_newSize);
    }

} //namespace spss
