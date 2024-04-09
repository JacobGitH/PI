CC= gcc 
CFLAGS= -pedantic -Wall -g 
LDFLAGS= -lm

SRC= $(shell find -name "*.c")
OBJ= $(addsuffix .o, $(basename $(SRC)))


all: program

run: all
		@./program

program:$(OBJ)
	$(CC) $(CFLAGS) -o  $@ $(OBJ) $(LDFLAGS)
	
clean:
	find -name "*.o" -delete
	rm program
