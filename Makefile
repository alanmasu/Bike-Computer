CC = gcc
CFLAGS = -Wall -g -DSIMULATE_HARDWARE

# Lista dei file .c da includere
C_SOURCES = main.c GPS.c

# Lista dei file .h da includere
H_HEADERS = GPS.h 

# Cartella per i file di build
BUILD_DIR = build

OBJECTS = $(addprefix $(BUILD_DIR)/, $(C_SOURCES:.c=.o))

TARGET = $(BUILD_DIR)/myprogram.exe

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	del /F /Q $(TARGET) $(addprefix $(BUILD_DIR)/, $(OBJECTS))
	rmdir /Q /S $(BUILD_DIR)