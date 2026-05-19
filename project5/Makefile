CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LIBS = -lsqlite3
TARGET = school_app

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c $(LIBS)

clean:
	rm -f $(TARGET)
