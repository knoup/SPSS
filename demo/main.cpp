#include <SPSS/System/Core.h>
#include <SPSS/System/MenuState.h>
#include <iostream>

void say(const std::string& _str) {
    std::cout << _str << std::endl;
}

void createSecondMenu(spss::Core& _c, sf::RenderWindow& _w, sf::Font& _f) {
    auto secondMenu{std::make_unique<spss::MenuState>(_w, _f, "Nothing to see here!")};
    secondMenu->addMenuItem("Take me back", std::bind(&spss::Core::popState, &_c));
    _c.pushState(std::move(secondMenu));
}

constexpr int CONSTANT_TIMESLICE{16};

int main(int argc, char* argv[]) {
    //Initialise our main window
    //--------------------------------------------------
    sf::RenderWindow window{sf::VideoMode(800, 600), "SPSS Demo"};
    //--------------------------------------------------

    //Load the font
    //--------------------------------------------------
    sf::Font font;
    font.loadFromFile("demo/18thcentury.ttf");
    //--------------------------------------------------

    //Initialise our core and menu state
    //--------------------------------------------------
    spss::Core core{window, CONSTANT_TIMESLICE};
    auto menuState{std::make_unique<spss::MenuState>(window, font, "SPSS Menu Demo")};
    //We can bind free functions to the menu items...
    menuState->addMenuItem("Say hi!", std::bind(say, "Hello there!"));
    //Wrap arguments in std::ref if you want to pass them by reference.
    menuState->addMenuItem("I want another menu!", std::bind(createSecondMenu, std::ref(core), std::ref(window), std::ref(font)));
    menuState->addGap();
    //...and also member functions.
    menuState->addMenuItem("Quit", std::bind(&spss::Core::exit, &core));

    core.pushState(std::move(menuState));
    core.run();
    //--------------------------------------------------

    return 0;
}
