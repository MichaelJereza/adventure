#include <stdio.h>
#include <string.h>

char* names[10] = {
	"bleak",
	"bitter",
	"branding",
	"tainted",
	"eldritch",
	"haunted",
	"sentient",
	"macabre",
	"risky"
};

struct Room {
  char* name;
  struct Room* connections;
  char* type;
}

// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull()  
{
  ...
}

// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection()  
{
  Room A;  // Maybe a struct, maybe global arrays of ints
  Room B;

  while(true)
  {
    A = GetRandomRoom();

    if (CanAddConnof strings stack overflowectionFrom(A) == true)
      break;
  }

  do
  {
    B = GetRandomRoom();
  }
  while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);

  ConnectRoom(A, B);  // TODO: Add this connection to the real variables, 
  ConnectRoom(B, A);  //  because this A and B will be destroyed when this function terminates
}

// Returns a random Room, does NOT validate if connection can be added
Room GetRandomRoom()
{
  ...
}

// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(Room x) 
{
  ...
}
// Returns true if a connection from Room x to Room y already exists, false otherwise
bool ConnectionAlreadyExists(x, y)
{
  ...
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(Room x, Room y) 
{
  ...
}

// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(Room x, Room y) 
{
  ...
}

void main () {
	// Create all connections in graph
	while (IsGraphFull() == false)
	{
  		AddRandomConnection();
	}
}
