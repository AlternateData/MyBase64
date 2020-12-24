CC 	= gcc
CFLAGS 	= -pedantic -Wall -Wextra -g -Og
LDFLAGS =
NAME 	= base64
SRC	= main.c
OBJ	= $(SRC:.c=.o)


all: options $(NAME)

options:
	@echo "options for making $(NAME)"
	@echo "    Source Files	$(SRC)"
	@echo "    Object Files	$(OBJ)"
	@echo "    CFLAGS		$(CFLAGS)"
	@echo "    LDFLAGS		$(LDFLAGS)"

.c.o:
	$(CC) -c $(CFLAGS) $<
.s.o:
	$(CC) -c $(CFLAGS) $<

$(NAME): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(NAME)


.PHONY: all options clean

