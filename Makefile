CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2
TARGET  = scheduler
SRC     = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) *.o *.out

.PHONY: all clean
