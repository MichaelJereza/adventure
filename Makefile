cc=gcc

adventure: jerezam.adventure.c buildrooms.o
	./buildrooms.o
	cc -lpthread jerezam.adventure.c -o adventure

buildrooms.o: jerezam.buildrooms.c
	cc -lpthread jerezam.buildrooms.c -o buildrooms.o

clean:
	rm -rf adventure buildrooms buildrooms.o jerezam.rooms.* currentTime.txt