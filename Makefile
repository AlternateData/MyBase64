CC=gcc
CFLAGS= -g -Wall
LFLAGS= -lm

base64 : main.o 
	$(CC) $(CFLAGS) $(LFLAGS) -o base64 main.o 

main.o : main.c 
	$(CC) $(CFLAGS) $(LFLAGS) -c main.c

clean :
	$(RM) main.o 
