CC       	= mpicc
CFLAGS		= -std=c11 -Wall -Wextra -g 
GTKFLAGS	= $(shell pkg-config --cflags gtk+-3.0)
GTKLIBS     	= $(shell pkg-config --libs gtk+-3.0)

SRC_DIR  	= src
INC_DIR  	= include
TEST_DIR 	= tests
BIN_DIR 	= bin
LOG_DIR 	= logs

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/coordinator.c \
       $(SRC_DIR)/client.c \
       $(SRC_DIR)/protocol.c \
       $(SRC_DIR)/gui.c \
       $(SRC_DIR)/cli.c \
       $(SRC_DIR)/mpi_wrapper.c

TARGET   = $(BIN_DIR)/mensajeria
TEST_BIN = $(BIN_DIR)/test_protocol

.PHONY: all clean test sanitize valgrind dirs

all: dirs $(TARGET)

dirs:
	mkdir -p $(BIN_DIR) $(LOG_DIR)

# -------------------------
# MAIN BUILD
# -------------------------
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(GTKFLAGS) -I$(INC_DIR) $(SRCS) -o $(TARGET) $(GTKLIBS)

# -------------------------
# TESTS (sin GTK)
# -------------------------
$(TEST_BIN): $(TEST_DIR)/test_protocol.c $(SRC_DIR)/protocol.c | dirs
	$(CC) $(CFLAGS) -I$(INC_DIR) \
	$(TEST_DIR)/test_protocol.c \
	$(SRC_DIR)/protocol.c \
	-o $(TEST_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

# -------------------------
# SANITIZE
# -------------------------
sanitize:
	$(CC) $(CFLAGS) $(GTKFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer \
	-I$(INC_DIR) $(SRCS) -o $(TARGET) $(GTKLIBS)

# -------------------------
# VALGRIND
# -------------------------
valgrind: $(TARGET) | dirs
	mpirun -np 3 \
		valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--error-exitcode=1 \
		--log-file=$(LOG_DIR)/valgrind-%p.log \
		./$(TARGET) User1 User2 User3 --test 2 10 || true

# -------------------------
# CLEAN
# -------------------------
clean:
	rm -rf $(BIN_DIR) $(LOG_DIR)
