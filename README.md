# About

## SPSS (Simple Program State System) 

is a modern C++ library built on top - and inspired by the style - of [SFML 2.5.1](https://www.sfml-dev.org/). A compiler that supports at least **C++17** is required to compile SPSS.

I found myself rewriting the same basic concepts across my personal projects, and thus decided to consolidate the ideas I've found work best for me into one convenient library. It also includes several modules that provide practical implementations of functionality that is useful in a variety of different applications. Some of its primary features include: 

* An intuitive and easy to use **state-based core** that handles the typical "game loop" (get input, update, draw) with a fixed timestep in an elegant manner. Most modules are also based on this same 3-function principle.

* A **MenuState** system that's very easy to use. It allows you to create simple menus and bind callback functions (only with a void return type, for now) to your menu items that are called upon clicking, so you can get started with the meat of your program right away rather than spending valuable time creating menus. 

* **ResourceManager**: a simple and straightforward class template to store resources. You can specify what type of resource you'd like to store, and what type of key you'd like to use to access it - generally, an enum class is a good idea.

* **DialogPrompt**: a draggable box which can be used for things like notifications. You can add buttons, with associated actions, as well as an optional text entry field. The size is automatically determined.

* **TextEntryBox**: a fully functional text entry box with most of the functionality you'd expect from one, including selecting multiple characters, skipping words, copying and pasting.

* **MulticolorText**: a modified version of sf::Text that removes the limitation of only being able to set one colour for the entire text object. You can not only specify different colours for different index ranges, but also outline colours and thicknesses.

* **Slider**: A horizontal slider that presents the user with multiple selectable values. A template class, the value type can be specified. The size of the inner bar will be automatically determined depending on the number of elements. The user can drag the inner bar, or click anywhere within the slider, to change the selected value.

* **Scrollbar**: A conventional scrollbar. Includes 3 anchor types:

	* None: no special behavior
	* Soft: if the scrollbar is already at the bottom-most position when reset() is called, it stays there.
	* Hard: the scrollbar is always set to the bottom-most position when reset() is called.

SPSS is a constantly evolving project that I'm always tweaking and adding new features to, as I come across the need for them.

# Usage

Here's a quick overview of the main modules and how they're meant to be used. More extensive documentation is available in the source code itself.

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

We can simply call `core.popState()` to remove the current state from the top of the stack and return to the previous state. If there are no more states, `core.exit()` is called.

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

Note that you can also specify an optional position	as the second argument of addMenuItem(). For example, we could've replaced the "Quit" entry with this:

`menuState->addMenuItem("Quit", {0.9, 0.9} std::bind(&spss::Core::exit, &core));`

to have the "Quit" button be positioned at the bottom right of the screen (90% X, 90% Y). If we don't specify this argument, position will be automatically determined.

Additionally, if there are more MenuItems than can fit on the screen, a scrollbar will be automatically activated.

![img](https://i.imgur.com/TmUWMYA.png)

See the demo for a more complete example.

## ResourceManager

Create a class that inherits from ResourceManager, specifying the resource type you want to store and the key type you'll use to access it. It's a good idea to also inherit from spss::Singleton, as you [presumably] only want one instance of whatever manager it is you're creating to exist in your program.

`class FontManager : public spss::ResourceManager<std::string, sf::Font>, public spss::Singleton<FontManager>`

Add resources (unique pointers):

```
FontManager manager;
auto f{std::make_unique<sf::Font>()};
f->loadFromFile("...");
manager.add("MyFont", std::move(f));
```

You can attempt to access a resource by calling get() with the desired key type as a parameter. This will return a const pointer to the element if it exists, and nullptr if it doesn't.

```
auto a{manager.get("MyFont")}; //Will return a const pointer to the font we created earlier
auto b{manager.get("qwerty")}; //Will return nullptr
```

## DialogPrompt

Initialize a prompt:

```
spss::DialogPrompt prompt{true,           //< enable text entry?
                          {600.F, 600.F}, //< position
                          font,           
                          "Prompt Title"};
```

Add buttons bounds to functions:

```
prompt.addButton("Ok", std::bind(...));
prompt.addButton("Cancel", std::bind(...));
```

Get the currently typed string (if text entry is disabled, returns a blank string):

`prompt.getString();`

![img](https://i.imgur.com/HEqI5LS.png)


## TextEntryBox

Initialise it with a sf::Font reference, character size, and 
your desired width:

`spss::TextEntryBox entry{font, 18, 500};`

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

Note that upon calling `setString()`, all current fill colors, outline colors, and outline thicknesses are reset.

## Slider

Initialize a slider. In this example, we'll use an enum class as our slider's value type.

`enum class Options {OPTION1, OPTION2, OPTION3};`

```
spss::Slider<Options> slider{{200,20},  //< size
                              {0, 0},   //< position
                              font,
                              "Size: "};
```

Add values:

```
slider.addValue(Options::OPTION1, "small");
slider.addValue(Options::OPTION2, "medium", true); //< This is the default value
slider.addValue(Options::OPTION3, "large");
```
![img](https://i.imgur.com/VUhDIqR.png)

## Scrollbar

Scrollbar is mostly used internally by certain modules, but you're free to use it in your own implementations as well. Usage is straightforward: simply give the constructor its requested arguments, and call `getInput() / update() / draw()`. To reset it, call `reset()`. For complete documentation, refer to _Scrollbar.h_.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License
[MIT](https://choosealicense.com/licenses/mit/)
