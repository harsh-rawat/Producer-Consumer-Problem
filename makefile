PROGNAME = prodcom
CC      = gcc
CFLAGS = -Wall -pedantic -Wextra
LDFLAGS = -pthread
OBJECTS = main.o Queue.o Threads.o statistics.o

all: $(PROGNAME)

$(PROGNAME): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(PROGNAME) $(OBJECTS)

main.o: main.c Queue.h Threads.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c main.c

statistics.o: statistics.c statistics.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c statistics.c

Queue.o: Queue.c Queue.h statistics.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c Queue.c

Threads.o: Threads.c Threads.h Queue.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c Threads.c

clean:
	rm -f $(OBJECTS) $(PROGNAME)