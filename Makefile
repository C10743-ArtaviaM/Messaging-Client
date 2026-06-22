CC       = mpicc
CFLAGS   = -std=c11 -Wall -Wextra -g $(shell pkg-config --cflags gtk+-3.0)
LIBS     = $(shell pkg-config --libs gtk+-3.0)

SRC_DIR  = src
INC_DIR  = include
TEST_DIR = tests

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/coordinator.c \
       $(SRC_DIR)/client.c \
       $(SRC_DIR)/protocol.c \
       $(SRC_DIR)/gui.c \
       $(SRC_DIR)/cli.c \
       $(SRC_DIR)/mpi_wrapper.c

TARGET   = mensajeria
TEST_BIN = test_protocol

.PHONY: all clean sanitize test

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -I$(INC_DIR) $^ -o $@ $(LIBS)

test:
	$(CC) -std=c11 -Wall -Wextra -g \
	$(TEST_DIR)/test_protocol.c \
	$(SRC_DIR)/protocol.c \
	-I$(INC_DIR) \
	-o $(TEST_BIN)

	./$(TEST_BIN)

sanitize: CFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
sanitize: $(TARGET)

clean:
	rm -f $(TARGET) $(TEST_BIN)