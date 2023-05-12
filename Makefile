#################################################
# Makefile variables
#################################################
CC := g++
LANG_STD := -std=c++17
COMPILER_FLAGS := -Wall -Wfatal-errors -g
INCLUDE_PATH := -I"./libs/"
SRC_COMPONENTS := ./src/Game/*.cpp \
				  ./src/Logger/*.cpp \
				  ./src/ECS/*.cpp \
				  ./src/AssetStore/*.cpp \
				  ./src/Utils/*.cpp

SRC_FILES 	:= 	./src/*.cpp $(SRC_COMPONENTS)
LINKER_FLAGS := -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.3

INCLUDE_PATH_TEST := -I"./src/Logger/" 
SRCFILES_TEST := ./src/tests/*.cpp

OBJ_NAME := gameengine
TEST_OBJ_NAME := gametest

#################################################
# Makefile rules
#################################################
build:
	$(CC) $(COMPILER_FLAGS) $(LANG_STD) $(INCLUDE_PATH) $(SRC_FILES) $(LINKER_FLAGS) -o $(OBJ_NAME)

test:
	$(CC) $(COMPILER_FLAGS) $(LANG_STD) $(INCLUDE_PATH) $(SRC_COMPONENTS) $(SRCFILES_TEST) $(LINKER_FLAGS) -o $(TEST_OBJ_NAME)
	./$(TEST_OBJ_NAME)

run:
	./$(OBJ_NAME)

clean:
	rm $(OBJ_NAME) gametest