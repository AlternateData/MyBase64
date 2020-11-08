CC 	= gcc
CFLAGS 	= -g -pedantic -Wall -Wextra
LDFLAGS =-lm
NAME 	= main
VERSION = 1.0
SRC	= main.c
OBJ	= $(SRC:.c=.o)


all: options $(NAME)

options:
	@echo "options for making $(NAME) $(VERSION)"
	@echo "    Source Files	$(SRC)"
	@echo "    Object Files	$(OBJ)"
	@echo "    CFLAGS		$(CFLAGS)"
	@echo "    LDFLAGS		$(LDFLAGS)"

debug: setdebugopts options $(NAME)

setdebugopts:
	$(eval CFLAGS+=-DDEBUG=1)

.c.o:
	$(CC) -c $(CFLAGS) $<
.s.o:
	$(CC) -c $(CFLAGS) $<

$(NAME): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(NAME)


.PHONY: all options clean debug setdebugopts

