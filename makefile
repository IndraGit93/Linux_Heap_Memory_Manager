CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 
UNITY_PATH = test/
OBJ_DIR = out
# Define the directory for object files

ifeq ($(CC),gcc)
CFLAGS += -Wno-unused-function -Wno-unused-variable
endif

CFLAGS += -I.
CFLAGS += -I$(UNITY_PATH)
CFLAGS += -I$(UNITY_PATH)/unity_src
LDFLAGS = -L$(UNITY_PATH)/unity_src

# Define the object files for the application
OBJ_APP = $(OBJ_DIR)/memory_manager.o $(OBJ_DIR)/app.o

# Define the object files for the unit tests
OBJ_TEST = $(OBJ_DIR)/memory_manager.o $(OBJ_DIR)/test_mm.o

# Default target, builds both the application and the unit tests
all: $(OBJ_DIR) test_mm app

# Create out/ directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Builds the test_mm executable for unit tests
test_mm: $(OBJ_TEST)
	$(CC) $(CFLAGS) $(LDFLAGS) -o test_mm $(OBJ_TEST) -lunity

# Builds the app executable for the application
app: $(OBJ_APP)
	$(CC) $(CFLAGS) -o app $(OBJ_APP)

# Compile memory_manager.c into memory_manager.o
$(OBJ_DIR)/memory_manager.o: memory_manager.c memory_manager.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile test_mm.c into test_mm.o
$(OBJ_DIR)/test_mm.o: test/test_mm.c memory_manager.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile app.c into app.o
$(OBJ_DIR)/app.o: app.c memory_manager.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean target
clean:
	rm -rf $(OBJ_DIR) test_mm app
