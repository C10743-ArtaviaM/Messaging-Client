CC       = mpicc
CFLAGS   = -std=c11 -Wall -Wextra -g $(shell pkg-config --cflags gtk+-3.0)
LIBS     = $(shell pkg-config --libs gtk+-3.0)

SRC_DIR  = src
INC_DIR  = include
TEST_DIR = tests
BIN_DIR = bin
LOG_DIR = logs

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/coordinator.c \
       $(SRC_DIR)/client.c \
       $(SRC_DIR)/protocol.c \
       $(SRC_DIR)/gui.c \
       $(SRC_DIR)/cli.c \
       $(SRC_DIR)/mpi_wrapper.c

TARGET   = $(BIN_DIR)/mensajeria
TEST_BIN = $(BIN_DIR)/test_protocol

.PHONY: all clean sanitize test valgrind

all: $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(LOG_DIR):
	mkdir -p $(LOG_DIR)

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

valgrind: $(TARGET) | $(LOG_DIR)
	mpirun -np 3 \
		valgrind \
		--leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--error-exitcode=1 \
		--log-file=$(LOG_DIR)/valgrind-%p.log \
		./$(TARGET) User1 User2 User3 --test 2 10 || true

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(LOG_DIR)