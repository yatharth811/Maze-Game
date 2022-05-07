FILE1 = server
FILE2 = player1
FILE3 = player2
CC = g++
COMPILER_FLAGS = -w
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2_mixer

all:
	$(CC) $(FILE1).cpp $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(FILE1).o
	$(CC) $(FILE2).cpp timer.cpp $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(FILE2).o
	$(CC) $(FILE3).cpp timer.cpp $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(FILE3).o

clean:
	rm -f *.o
