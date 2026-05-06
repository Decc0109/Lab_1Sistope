CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm
TARGET = lab1
SRC = lab1.c funciones.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c funciones.h
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)
