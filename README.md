# Rayzor

A top-down shooter where your only weapon is light. Colored enemies can only be defeated with matching light.

## Controls
* Move with WASD, use the mouse to aim and shoot. Hold space, move and release space to place a wall.
* Press ESC to pause and buy upgrades.
* Press R to Reload a level.

![Demo](rayzor.gif)
## Requirements
### Linux
* Install `libsfml-dev` and `libtgui-dev` (Ubuntu: TGUI is in a PPA).
### Windows
* Build SFML and TGUI and change the paths in CMakeLists.txt accordingly.

## Build and run
```
mkdir build
cd build
cmake .
make
cd ..
build/Rayzor
```
* Start the game with `-16` to enable 16xAA. Does not seem to work on AMD GPUs.
### Notes
* The level files were made with https://github.com/bjorn/tiled, the files are in res/levels
* The parsing of the level-files inside of Rayzor is dirty and propably not safe.

### Credits

* Inspired by https://github.com/usercynical/2D-RayCasting

* Contains textures from Kenney.nl:

````
	Topdown (Shooter) Pack

	by  Kenney Vleugels (Kenney.nl)

			------------------------------

	License (Creative Commons Zero, CC0)
	http://creativecommons.org/publicdomain/zero/1.0/

	You may use these assets in personal and commercial projects.
	Credit (Kenney or www.kenney.nl) would be nice but is not mandatory.

			------------------------------

	Donate:   http://support.kenney.nl
	Request:  http://request.kenney.nl

	Follow on Twitter for updates:
	@KenneyWings
````