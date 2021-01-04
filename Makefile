CC 	    = gcc
CFLAGS 	= -pedantic -Wall -Wextra -g -Og
LDFLAGS =
NAME 	= base64
SRC	    = main.c
OBJ	    = $(SRC:.c=.o)


all: $(NAME)

.c.o:
	$(CC) -c $(CFLAGS) $<

$(NAME): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(NAME)


.PHONY: all clean

