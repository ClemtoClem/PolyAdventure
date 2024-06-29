# PolyAdvanture Game

## General Info

This project was developed for the school purpose of developing a video game in one month.

We chose to develop a 2D action-adventure game. Our idea was to create an environment in which the character can interact with several entities such as watches or NPCs.

The game uses tile sets for the graphics of the world and entities.

To facilitate the creation of new worlds and entities, configuration files allow simple and dynamic modification of the game, without the need to recompile it. The configuration files, recognizable by their '.conf' extension, use a syntax that we have developed especially for the game. This syntax allows you to indicate the type of each variable, to construct tables and to define variables from their digital identifier.

The management of game resources is managed by a large linked list which imports resources as they are requested, which makes it possible to limit the use of memory with unused resources.

A debugger mode is available and can be activated on the command line by entering './PolyAdvanture -debug' in the terminal.

The interaction between entities uses a rudimentary system. Each entity has two collision boxes, one to define the size of its body and one to define the part of its body in contact with the ground. Each entity is stored in the same list. At each frame, the entities are updated, which allows them to be moved automatically and displayed one according to the other to give an interesting perspective if the collision boxes of two entities overlap.

This interaction is visible in the screenshots below between the player and a dragon.

The game is still at the very beginning of its development, the game mechanics still require reflection.

Our main ambition was to create a game from A to Z using no external libraries, apart from the SDL2 graphics library. So we set about creating our own GUI system to display text panels and buttons. To give a retro style each widget needs a set of tiles on an image for its display. The size of a tile must be at least 16x16 pixels for aesthetic graphics, but the most important thing is to respect the number of tiles in length and width on the image according to the request of the widget.

## Keyborad

Use the arrow keys on your keyboard to move the character, the space key to read signs and open doors and the + and - keys to enlarge or reduce the view of the map.

## Technologies

The programs use the SDL2 library for sound and graphics.

This project was developed under Linux with the vscode editor.

The code was completed with version gcc 11.4.0.

## Captures

![screenshot](screenshots/screenshot1.png)

![screenshot](screenshots/screenshot2.png)

![screenshot](screenshots/screenshot3.png)

![screenshot](screenshots/screenshot4.png)