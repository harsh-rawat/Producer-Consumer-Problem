PROGNAME = prodcom
CC      = gcc
CFLAGS = -Wall -pedantic -Wextra
LDFLAGS = -pthread
OBJECTS = main.o Queue.o Threads.o statistics.o Error.o

all: $(PROGNAME)

$(PROGNAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PROGNAME) $(OBJECTS)

main.o: main.c Queue.h Threads.h Error.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c main.c

statistics.o: statistics.c statistics.h Error.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c statistics.c

Queue.o: Queue.c Queue.h statistics.h Error.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c Queue.c

Threads.o: Threads.c Threads.h Queue.h Error.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c Threads.c

Error.o: Error.c Error.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c Error.c

clean:
	rm -f $(OBJECTS) $(PROGNAME)