# About

SPSS (Simple Program State System) is a modern C++ library built on top of [SFML 2.5.1](https://www.sfml-dev.org/). Its primary purpose is to provide an intuitive and easy to use **state-based core** that handles the typical "game loop" (get input, update, draw) with a fixed timestep in an elegant manner. I found myself rewriting the same basic concept across projects and thus decided to create this libary.

In addition, SPSS contains a **MenuState** system that's very easy to use. It allows you to create simple menus and bind callback functions (with a void return type) to your menu items that are called upon clicking, so you can get started with the meat of your program right away rather than spending valuable time creating menus. 

SPSS also contains some other useful modules, such as:

**TextEntryBox**: a fully functional text entry box with most of the functionality you'd expect from one, including selecting multiple characters, skipping words, copying and pasting.

**MulticolorText**: a modified version of sf::Text that removes the limitation of only being able to set one colour for the entire text object. You can not only specify different colours for different index ranges, but also outline colours and thicknesses.

With time, more useful features will be added.

A compiler that supports at least C++17 is required to compile SPSS.

# Usage

## Core/States

SPSS is based on the idea of a **Core** and **States**. A core is constructed with a sf::RenderWindow reference and a fixed timestep value:

`spss::Core core{window, 16};`

spss::State is an abstract class that the user is meant to derive their desired states from. Overloads are required for the destructor, `getInput()`, `update()`, and `draw()`. Once a user has defined their own state, they can push it to the core's state stack like so:

```
auto state{std::make_unique<SampleState>(window)};
core.pushState(std::move(state));
```

We can then call `core.run()`, which will begin the program loop. By default, it performs the `getInput()` -> `update()` -> `draw()` loop using a fixed timestep, calling the state at the top of the state stack's versions of those functions. It can be overloaded should the user require more specific fine control over the loop.

Sometimes, we'll want to still draw the previous state behind the current state, such as in a pause menu. This is possible by calling State's `setDrawnInBackground()` (also see `setUpdatedInBackground()` and `setGetInputInBackground()`).

## MenuState

spss::MenuState provides a quick way to set up simple and functional menus. 

```
void say(const std::string& _str) {
	std::cout << _str << std::endl;
}

int main() {
	auto menuState{std::make_unique<spss::MenuState>(window, font, "Title Text")};

	//We can bind functions to menu items and provide an optional keyboard shortcut
	menuState->addMenuItem("Why, hello there!", std::bind(say, "Hi"), sf::Keyboard::H);

	menuState->addMenuItem("This does nothing");
	menuState->addGap();

	//We can also bind member functions
	menuState->addMenuItem("Quit", std::bind(&spss::Core::exit, &core));

	core.pushState(std::move(menuState));
	core.run();
}
```

![img](https://i.imgur.com/TmUWMYA.png)

See the demo for a more complete example.

## TextEntryBox

Initialise it with a sf::Font reference and your desired width:

`spss::TextEntryBox entry{font, 500};`

Use setActive() to enable text entry when desired:

`entry.setActive(true);`

Incorporate getInput(), update(), and draw() with the rest of your program.

```
entry.getInput(event);
entry.update();
entry.draw(window);
```

Hold SHIFT and use the arrow keys to select a sequence. Hold CTRL to skip words.

![img](https://i.imgur.com/nS5mpXy.png)

![img](https://i.imgur.com/bHvac0k.png)

Upon pressing RETURN, the contents of the box are cleared and the box becomes inactive (unless you've called `setAlwaysActive(true)`). You can get the last entered string by calling `getLastString()`. You can get the current contents of the box by calling `getCurrentString()`.

## MulticolorText

Usage is identical to sf::Text, except `setFillColor()`, `setOutlineColor()`, and `setOutlineThickness()` can also be given optional start and end pos arguments.

Note that upon calling setString(), all current fill colors, outline colors, and outline thicknesses are reset.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License
[MIT](https://choosealicense.com/licenses/mit/)
