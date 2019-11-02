Adventure is a simple C text adventure game.

### Installation
`make` runs the Makefile provided to compile and run the initial buildrooms script

`make clean` to clean the build directory run 

### Generating Rooms
The Makefile will initially run the `buildrooms.o` script generating a directory of rooms.

Executing `buildrooms.o` again will generate a new directory of rooms.

### Gameplay
`adventure` begins the game with the latest generate room directory.

The syntax of the game is simple:
- Rooms are identified by their name.
- There are randomly generated paths from one room to others, the amount of connections varies.
- User begins in a random room.
- Traverse to another room by typing the name of the connection.
- The goal is to reach a randomly selected end room.
- Upon reaching the end, the successfuly path is printed with the amount of steps taken.


#### This program also implements a multi-threaded clock
- `time` will print the current system time.
- It will also write to currentTime.txt
