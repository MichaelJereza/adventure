#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>

struct Room {
    char* name;
    int doors; //count of connections
    struct Room* connections[6];
    char* type;
};

// thread 
pthread_t time_thread;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 
// mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


// Searches through directory, finding newest one
// Citing code from Lec 1.6 slide 6
void getNewestDir(char* output_buffer){
    int time = -1;
    char prefix[] = "jerezam.rooms.";
    char dirName[60];
    memset (dirName, '\0', sizeof(dirName)); // Initializes string with null terminator
    
    DIR* checkDir;
    struct dirent* current_file;
    struct stat attr;

    checkDir = opendir(".");

    while((current_file = readdir(checkDir)) != NULL){
        if(strstr(current_file->d_name, prefix) != NULL){ //if prefix
            stat(current_file->d_name, &attr);
            if (attr.st_mtime > time){
                time = attr.st_mtime;
                memset (dirName, '\0', sizeof(dirName)); // Reinitialize string each time
                strcpy(dirName, current_file->d_name);
            }
        }
    }
    closedir(checkDir);
    strcpy(output_buffer, dirName);
    return;
}

// Generate Room pointer array to dynamically allocated memory
// Contents from files ROOM_[1-7]
struct Room** getRooms(){
    struct Room** rooms;
    char buffer[30];
    size_t bfsz = 0;
    char* lineptr = NULL;
    char room_number;
   
    char directory[30];
    memset (directory, '\0', sizeof(directory));

    getNewestDir(directory);
    
    rooms = malloc(7*sizeof(struct Room*));
    int i;
    for(i = 0; i < 7; i++){
        // Create memory for room[i]
        rooms[i] = malloc(sizeof(struct Room));
        rooms[i]->name = malloc(sizeof(char[8]));
        rooms[i]->doors = 0;
        
        // Directory path into buffer
        memset (buffer, '\0', sizeof(buffer));
        strcpy(buffer, directory);

        // File path into buffer
        strcat(buffer, "/ROOM_");
        room_number = (i+1) + '0';
        strncat(buffer, &room_number, 1);

        FILE* room_stream = fopen(buffer, "r"); //using buffer as the file path

        // Set position of beginning of name
        int pos = fseek(room_stream, 11, SEEK_CUR);
        // if(lineptr)
        getline(&lineptr, &bfsz, room_stream);

        // Find newline and add null terminator
        int newline;
        for(newline = 0; newline < bfsz; newline++){
            if (lineptr[newline]=='\n'){
                lineptr[newline]='\0';
            }
        }
        // Add name to rooms[i]
        // and free lineptr
        strcpy(rooms[i]->name, lineptr);
        fclose(room_stream);
    }
    // Rooms allocated and have names
    
    // Now add connections and types
    int c;
    for(c = 0; c < 7; c++){
        // Directory path into buffer
        memset (buffer, '\0', sizeof(buffer));
        strcpy(buffer, directory);
        
        // File path into buffer
        strcat(buffer, "/ROOM_");
        room_number = (c+1) + '0';
        strncat(buffer, &room_number, 1);

        FILE* room_stream = fopen(buffer, "r"); //using buffer as the file path        
        
        ssize_t read;
        int con_num = 0; // Iterates based on connections number
        while(read = getline(&lineptr, &bfsz, room_stream) != -1){
            
            // Set connection number
            if((lineptr[11] == '1')||(lineptr[11] == '2')||(lineptr[11] == '3')||(lineptr[11] == '4')||(lineptr[11] == '5')||(lineptr[11] == '6')){
                con_num = lineptr[11]-'0';
            }

            // If line is a connection
            if(con_num!=0){
                // For end of format to end of name
                int iter;
                for(iter = 14; iter < strlen(lineptr); iter++){
                    // If iter is last char
                    // Append \0
                    if(iter == strlen(lineptr)-1){
                        buffer[iter-14] = '\0';


                    }
                    else{
                        buffer[iter-14] = lineptr[iter];
                    }
                }     
                // Name from string in buffer
                char name[8];
                struct Room* sought = NULL;
                strcpy(name, buffer);
                
                // For connections, if name, pointer to room in sought
                int room_seek;
                for(room_seek = 0; room_seek < 7; room_seek++){
                    if(strcmp(rooms[room_seek]->name, name)==0){
                        sought = rooms[room_seek];
                        room_seek=7;
                    }
                }
                // Add sought to connections
                rooms[c]->connections[con_num-1] = sought;
                rooms[c]->doors++;
                con_num = 0;
            }
            // If line is type
            else if(lineptr[5]=='T'){
                int typ_iter;
                for(typ_iter = 11; typ_iter <= strlen(lineptr); typ_iter++){
                    // Append \0
                    if(typ_iter == strlen(lineptr)){
                        buffer[typ_iter-11] = '\0';
                    }
                    else{
                        buffer[typ_iter-11] = lineptr[typ_iter];
                    }
                }
                // Add type to room
                rooms[c]->type = malloc(sizeof(char[8]));
                strcpy(rooms[c]->type, buffer);
            }  
        }
        fclose(room_stream);
    }
    free(lineptr);
    return rooms;
}

// Deallocates memory used by room array.
void freeRooms(struct Room** rooms){
    int i;
    for(i = 0; i < 7; i++){
        free(rooms[i]->name);
        free(rooms[i]->type);
        free(rooms[i]);
    }
    free(rooms);
    return;
}
// Prompt for input, 
// if location_prompt=1 location and connections printed
void prompt(struct Room* location, int location_prompt){
    if(location_prompt==1){
        printf("\nCURRENT LOCATION: %s\n", location->name);
        printf("POSSIBLE CONNECTIONS: ");
        int d;
        for(d = 0; d < location->doors; d++){
            printf("%s", location->connections[d]->name);
            if(d == location->doors-1){
                printf(".\n");
            }
            else{
                printf(", ");
            }
        }
    }

    printf("WHERE TO? >");
    return;
}
// Gets string from stdin
// Assures returned string is room name or time
char* getValidInput(struct Room* location, int prompt_setting){
    char* buffer = NULL;
    size_t bfsz = 0;
    bool valid = false;
    do{
        prompt(location, prompt_setting);         

        getline(&buffer, &bfsz, stdin);
        buffer[strlen(buffer)-1]='\0';

        if(strcmp(buffer, "time")==0){
            valid = true;
            break;
        }
        else{
            bool huh = true;
            int con;
            for(con = 0; con < location->doors; con++){
                if(strcmp(buffer, location->connections[con]->name)==0){
                    valid = true;
                    huh = false;
                    break;
                }
            }
            if(huh){
                printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
                prompt_setting = 1;
            }
        }
    }while(!valid);
    return buffer;
}

// Prints formatted date string and writes to currentTime.txt
void* getTime(){
    
    // Wait for stdin "time"
    pthread_cond_wait(&cond, &lock);
    
    // Citing strftime manual
    time_t t = time(NULL);
    struct tm* tmp;
    tmp = localtime(&t);
    char buffer[60];
    memset (buffer, '\0', sizeof(buffer)); // Initializes string with null terminator
    
    // Formatted date into buffer
    strftime(buffer, sizeof(buffer),"%l:%M%P, %A, %B %d, %Y",tmp);
    printf("\n%s\n\n", buffer);
    
    // Open output file and write buffer
    FILE* stream = fopen("currentTime.txt", "w+");
    fwrite(buffer, sizeof(char), strlen(buffer), stream);
    fclose(stream);

    return 0;
}

void play(struct Room** map){
    struct Room* cur_room = map[0];
    char** history = NULL;
    int steps = 0;
    char* choice = NULL;
    int loc_prompt = 1;
    // Loops until cur_room is the end
    while(strcmp(cur_room->type, "END_ROOM")!=0){
        choice = getValidInput(cur_room, loc_prompt);
        // If time is input
        if(strcmp(choice, "time")==0){
            loc_prompt = 0;
            pthread_cond_signal(&cond);
            pthread_join(time_thread, NULL);
           
            // Recreate thread
            pthread_create( &time_thread, NULL, getTime, NULL );
        }
        // Add name to history and increment steps
        // Change cur_room to name
        else{
            int con;
            for(con = 0; con < cur_room->doors; con++){
                if(strcmp(choice, cur_room->connections[con]->name)==0){
                    cur_room = cur_room->connections[con];
                    history = realloc(history, (steps+1)*sizeof(history[8]));
                    history[steps] = cur_room->name;
                    steps++;
                }
            }
            loc_prompt = 1;
        }
        free(choice);
    }
    printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
    int s;
    for(s = 0; s < steps; s++){
        printf("%s\n", history[s]);
    }
    free(history);
}

void main(){
    struct Room** the_rooms = getRooms();
    
    // Create pthread
    pthread_create( &time_thread, NULL, getTime, NULL );

    // Start the game
    play(the_rooms);

    // Clean up
    pthread_mutex_destroy(&lock);
    freeRooms(the_rooms);
    return;
}