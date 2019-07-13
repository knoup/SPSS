////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "SPSS/System/Core.h"

namespace spss {

    ////////////////////////////////////////////////////////////
    Core::Core(sf::RenderWindow& _window,
               const int _timeslice)
        : m_window{_window}, m_timeslice{_timeslice}, m_stateStack{} {
    }

    ////////////////////////////////////////////////////////////
    void Core::run() {
        int       simulationTime{0}; //as milliseconds
        sf::Clock timesliceClock{};

        while (m_window.isOpen() && !m_stateStack.empty()) {
            m_window.clear(sf::Color(0, 0, 0));
            getInput();
            int realTime{timesliceClock.getElapsedTime().asMilliseconds()};
            while (simulationTime < realTime) {
                simulationTime += m_timeslice;
                update(m_timeslice);
            }
            draw();
        }
    }

    ////////////////////////////////////////////////////////////
    void Core::getInput() {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window.close();
            }
            if (m_stateStack.size() > 1) {
                if(m_stateStack.end()[-2]->getInputInBackground()) {
                    m_stateStack.end()[-2]->getInput(event);
                }
            }
            m_stateStack.back()->getInput(event);
        }
    }

    ////////////////////////////////////////////////////////////
    void Core::update(int _timeslice) {
        if (m_stateStack.size() > 1) {
            if(m_stateStack.end()[-2]->updatedInBackground()) {
                m_stateStack.end()[-2]->update(_timeslice);
            }
        }
        m_stateStack.back()->update(_timeslice);
    }

    ////////////////////////////////////////////////////////////
    void Core::draw() {
        if (m_stateStack.size() > 1) {
            if(m_stateStack.end()[-2]->drawnInBackground()) {
                m_stateStack.end()[-2]->draw();
            }
        }
        m_stateStack.back()->draw();

        m_window.display();
    }

    ////////////////////////////////////////////////////////////
    void Core::pushState(std::unique_ptr<spss::State> _state) {
        m_stateStack.push_back(std::move(_state));
    }

    ////////////////////////////////////////////////////////////
    void Core::popState() {
        m_stateStack.pop_back();

        if (!m_stateStack.empty()) {
            m_stateStack.back()->onStateSwitch();
        }
    }

    ////////////////////////////////////////////////////////////
    void Core::exit() {
        m_window.close();
    }

    ////////////////////////////////////////////////////////////
    const State* Core::getTopState() const {
        if(m_stateStack.empty()) {
            return nullptr;
        }
        return m_stateStack.back().get();
    }

} //namespace spss
