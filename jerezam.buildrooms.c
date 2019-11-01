#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <fcntl.h>

struct Room {
  char* name;
  int doors; //count of connections
  struct Room* connections[6];
  char* type;
};

// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull(struct Room** all_rooms)  
{
  for(int i=0; i<7; i++){
    int c=all_rooms[i]->doors;
    if((c<3)||(c>6)){
      return false;
    }
  }
  return true;
}


// Returns a random Room, does NOT validate if connection can be added
struct Room* GetRandomRoom(struct Room** array)
{
  int l = rand() % 7;
  return (array[l]);
}

// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(struct Room x) 
{
  if(x.doors < 6){
    return true;
  }
  else{
    return false;
  }
}

// Returns true if a connection from Room x to Room y already exists, false otherwise
bool ConnectionAlreadyExists(struct Room* x, struct Room* y)
{
  for(int i = 0; i < x->doors; i++){
    if(x->connections[i]->name==y->name){
      return true;
    }
  }
    return false;
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct Room* x, struct Room* y) 
{
  if(x->doors == 0){
    x->connections[0] = y;
  }
  else{
    x->connections[x->doors] = y;
  }
  x->doors++;
}

// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(struct Room x, struct Room y) 
{
  if(x.name==y.name){
    return true;
  }
  else{
    return false;
  }
}


// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection(struct Room** rooms_ptr)  
{
  struct Room* A;
  struct Room* B;

  while(true)
  {
    A = GetRandomRoom(rooms_ptr);

    if (CanAddConnectionFrom(*A) == true)
      break;
  }

  do
  {
    B = GetRandomRoom(rooms_ptr);
  }  while(CanAddConnectionFrom(*B) == false || IsSameRoom(*A, *B) == true || ConnectionAlreadyExists(A, B) == true);
 
  ConnectRoom(A, B); 
  ConnectRoom(B, A);
}

// Assigns random Room names. Assign type 5 MID_ROOMS, START_ROOM, & END_ROOM.
// Pre: Allocated array of 7 Room pointers
// Post: 7 Randomly generated names and room types.
void asnRooms (struct Room** new_rooms) {
  char* names[] = {
    "bleak",
    "bitter",
    "branding",
    "tainted",
    "eldritch",
    "haunted",
    "sentient",
    "macabre",
    "risky",
    "rigid"
  };
  int picks = 10;

  for(int i=0; i<7; i++){
    int r = ( rand() % picks );
    
    new_rooms[i]->name = names[r];
    new_rooms[i]->type = "MID_ROOM";

    for( int j = r; j < picks; j++ ){
      names[j] = names[j+1];
    }
    new_rooms[i]->doors = 0;
    picks--;
  }
  new_rooms[0]->type = "START_ROOM";
  new_rooms[6]->type = "END_ROOM";
}


// Pre: Array of 7 assigned and connected rooms
// Post: 7 files names ROOM_(1-7)
void outputRooms (struct Room** final_rooms){
  char num;
  for(int i = 0; i < 7; i++){
    
    // Create directory
    int p = getpid();
    char directory[30];
    sprintf(directory, "jerezam.rooms.%d\0", p);
    mkdir(directory, 0777);

    // File name
    char path[6];
    strcat(directory, "/ROOM_");
    num = i + '1';

    // Get file
    strncat(directory, &num, 1);
    //strcat(directory, path);
    FILE* output_file = fopen(directory, "w+");
    char buffer[30];

    // Write name
    strcpy(buffer, "ROOM NAME: ");
    strcat(buffer, final_rooms[i]->name);
    strcat(buffer, "\n");
    fwrite(buffer, sizeof(char), strlen(buffer), output_file);
    
    // Write connections
    for(int d = 1; d < final_rooms[i]->doors+1; d++){
      num = d + '0';
      char* name = final_rooms[i]->connections[d-1]->name;
      strcpy(buffer, "CONNECTION "); 
      strncat(buffer, &num, 1 ); strcat(buffer, ": "); strcat(buffer, name); strcat(buffer, "\n");
      fwrite(buffer, sizeof(char), strlen(buffer), output_file);
    }

    // Write room type
    strcpy(buffer, "ROOM TYPE: ");
    strcat(buffer, final_rooms[i]->type);
    fwrite(buffer, sizeof(char), strlen(buffer), output_file);
  }
}

void main () {

  struct Room* graph[7];

  // Generate Rooms
  srand(time(0));
  for(int i = 0; i < 7; i++){
    graph[i]=malloc(sizeof(struct Room));
  }
  asnRooms(graph);

	// Create all connections in graph
  while (IsGraphFull(graph) == false)
	{
      AddRandomConnection(graph); 
	}

  outputRooms(graph);
  // Free allocated memory
  for(int i = 0; i < 7; i++){
    free(graph[i]);
  }
}
