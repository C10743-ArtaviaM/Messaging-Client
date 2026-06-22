CC       = mpicc
CFLAGS   = -std=c11 -Wall -Wextra -g $(shell pkg-config --cflags gtk+-3.0)
LIBS     = $(shell pkg-config --libs gtk+-3.0)

SRC_DIR  = src
INC_DIR  = include
TEST_DIR = tests
BIN_DIR = bin

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/coordinator.c \
       $(SRC_DIR)/client.c \
       $(SRC_DIR)/protocol.c \
       $(SRC_DIR)/gui.c \
       $(SRC_DIR)/cli.c \
       $(SRC_DIR)/mpi_wrapper.c

TARGET   = $(BIN_DIR)/mensajeria
TEST_BIN = $(BIN_DIR)/test_protocol

.PHONY: all clean sanitize test

all: $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(SRCS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) $^ -o $@ $(LIBS)

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_DIR)/test_protocol.c $(SRC_DIR)/protocol.c | $(BIN_DIR)
	$(CC) -std=c11 -Wall -Wextra -g \
	$(TEST_DIR)/test_protocol.c \
	$(SRC_DIR)/protocol.c \
	-I$(INC_DIR) \
	-o $(TEST_BIN)

sanitize: CFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
sanitize: $(TARGET)

clean:
	rm -rf $(BIN_DIR)