CC=gcc
CPPFLAGS=-Wall -g -O3 -std=gnu99 -Wno-unused-result


SRC=src/certcheck.o
TARGET=certcheck

all: $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CPPFLAGS) -lm

clean:
	rm -f $(TARGET) src/*.o
