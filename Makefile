CC     = g++
CFLAGS = -Wall -Wextra -O2 -std=gnu17
LDFLAGS =

.PHONY: all clean

TARGET1 = client
TARGET2 = server

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(TARGET1).o err.o
$(TARGET2): $(TARGET2).o err.o

# To są zależności wygenerowane automatycznie za pomocą polecenia `gcc -MM *.c`.
client.o: client.cpp err.h
server.o: server.cpp err.h
err.o: err.cpp err.h

clean:
	rm -f $(TARGET1) $(TARGET2) *.o *~
