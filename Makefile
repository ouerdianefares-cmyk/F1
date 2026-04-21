CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS =

SRC = main.c board.c game.c display.c save.c
OBJ = $(SRC:.c=.o)
EXEC = cy_connect

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c board.h game.h display.h save.h
board.o: board.c board.h
game.o: game.c game.h board.h display.h save.h
display.o: display.c display.h board.h
save.o: save.c save.h board.h

clean:
	rm -f $(OBJ) $(EXEC)

rebuild: clean all

.PHONY: all clean rebuild
