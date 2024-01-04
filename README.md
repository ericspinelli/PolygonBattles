# README.md

## Installation

Requires SFML: `sudo apt-get install libsfml-dev`

Compile: `g++ -c *.cpp && g++ *.o -o polygons -lsfml-graphics -lsfml-window -lsfml-system`

## Controls

* **Movement:** WASD (Up, Left, Down, Right)

* **Weapons:** Left mouse (bullets), Right mouse (special weapon w/ cooldown)

* **Pause:** P (pause/unpause)

* **Quit:** ESC

# Gameplay

Shoot the polygons that appear for 100 points for each side of the polygon.  Shooting a polygon explodes it into a number of smaller polygons equal to the number of sides, each worth double what the larger polygon was worth.

The player has 3 lives.  Dying immediately respawns the player in the center position.  Player is invulnerable for a short duration after each death.

Enemy polygons get faster on every even number level.  Additional polygons spawn on every odd number level.
