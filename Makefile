CC=gcc
OPENSSLFLAGS=-I /usr/local/opt/openssl/include -L /usr/local/opt/openssl/lib -lssl -lcrypto
CPPFLAGS=-Wall -g -O3 -std=gnu99 -Wno-unused-result -Wno-unused-command-line-argument $(OPENSSLFLAGS)

SRC=src/certcheck.o src/csv_helper.o src/cert_helper.o
TARGET=certcheck

all: $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CPPFLAGS)

clean:
	rm -f $(TARGET) src/*.o
