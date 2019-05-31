CC=g++
CFLAGS=-g -Wall -pedantic -pthread

all: tests-drivers

tests-drivers: tests-drivers.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -o $@

clean:
	rm -rf *.o
	rm -rf tests-drivers
