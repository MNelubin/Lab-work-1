# Configuration
PROJECT = lgo
LIBPROJECT = $(PROJECT).a
TESTPROJECT = test-$(PROJECT)

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests 

# Tools
CXX = g++
AR = ar
ARFLAGS = rcs

# Flags
# Base CXXFLAGS
CXXFLAGS_BASE = -I$(INC_DIR) -std=c++17 -Wall -g -fPIC -Werror -Wpedantic
# OpenMP flags
OMPFLAGS = -fopenmp -O3
# Application specific CXXFLAGS
CXXFLAGS = $(CXXFLAGS_BASE) $(OMPFLAGS) -DPROJECT_NAME="\"$(PROJECT)\""

# Linker flags
# Base LDLIBS
LDLIBS_BASE = -lpthread
LDAPPFLAGS = $(OMPFLAGS) $(LDLIBS_BASE)
LDGTESTFLAGS = $(OMPFLAGS) $(LDLIBS_BASE) -lgtest -lgtest_main

# Source Files
APP_SRC = main.cpp # main.cpp is in the root directory
LIB_SRC = $(SRC_DIR)/ImD.cpp
TEST_SRC_FILES = $(TEST_DIR)/test-$(PROJECT).cpp  


# Object Files
APP_OBJ = $(notdir $(APP_SRC:.cpp=.o))
LIB_OBJ = $(notdir $(LIB_SRC:.cpp=.o))
TEST_OBJ = $(notdir $(TEST_SRC_FILES:.cpp=.o)) 

# Dependencies
DEPS = $(wildcard $(INC_DIR)/*.h)

# Targets
.PHONY: all test clean cleanall info

default: all

all: info $(PROJECT)

info:
	@echo "Building project $(PROJECT) with OpenMP enabled..."

# Build application
$(PROJECT): $(APP_OBJ) $(LIBPROJECT)
	@echo "Linking application $(PROJECT)..."
	$(CXX) $(CXXFLAGS) -o $@ $(APP_OBJ) $(LIBPROJECT) $(LDAPPFLAGS)

# Build library
$(LIBPROJECT): $(LIB_OBJ)
	@echo "Creating library $(LIBPROJECT)..."
	$(AR) $(ARFLAGS) $@ $^

# Build tests
$(TESTPROJECT): $(TEST_OBJ) $(LIBPROJECT)
	@echo "Linking test executable $(TESTPROJECT)..."
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_OBJ) $(LIBPROJECT) $(LDGTESTFLAGS)

test: $(TESTPROJECT)
	@echo "Running unit tests (if $(TESTPROJECT) exists and is runnable)..."
	@if [ -f ./$(TESTPROJECT) ]; then ./$(TESTPROJECT); else echo "Test executable '$(TESTPROJECT)' not found or not runnable. Ensure 'test-Transformers.cpp' exists and compiles."; fi

# Rule to compile application source file (main.cpp from root)
$(APP_OBJ): %.o: %.cpp $(DEPS)
	@echo "Compiling $< -> $@..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Rule to compile library source files (from SRC_DIR)
$(LIB_OBJ): %.o: $(SRC_DIR)/%.cpp $(DEPS)
	@echo "Compiling $< -> $@..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Rule to compile test source file (from TEST_DIR)
$(TEST_OBJ): %.o: $(TEST_DIR)/%.cpp $(DEPS) 
	@echo "Compiling test source $< -> $@..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean rules
clean:
	@echo "Cleaning object files..."
	rm -f $(APP_OBJ) $(LIB_OBJ) test-$(PROJECT).o

cleanall: clean
	@echo "Cleaning executables and library..."
	rm -f $(PROJECT) $(LIBPROJECT) $(TESTPROJECT)
	rm -f massif*

