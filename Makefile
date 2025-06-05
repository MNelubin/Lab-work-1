# Configuration
PROJECT = lgo
LIBPROJECT = $(PROJECT).a
TESTPROJECT = test-$(PROJECT)

# Directories
SRC_DIR = src
TEST_DIR = tests
INC_DIR = include
BIN_DIR = out

# Tools
CXX = g++
AR = ar
ARFLAGS = rcs


GTEST_CFLAGS ?= $(shell pkg-config --cflags gtest 2>/dev/null)
GTEST_LIBS   ?= $(shell pkg-config --libs gtest 2>/dev/null)

# Flags
# Base CXXFLAGS
CXXFLAGS_BASE = -I$(INC_DIR) -std=c++17 -Wall -g -fPIC -Werror -Wpedantic
# Coverage CXXFLAGS
CXXFLAGS_COVERAGE = $(CXXFLAGS_BASE) -fprofile-arcs -ftest-coverage
# OpenMP flags
OMPFLAGS = -fopenmp -O3 
# Application specific CXXFLAGS
CXXFLAGS_COMPILE = $(CXXFLAGS_BASE) $(OMPFLAGS) -DPROJECT_NAME=\"$(PROJECT)\" -isystem /usr/lib/gcc/x86_64-linux-gnu/13/include $(GTEST_CFLAGS)
CXXFLAGS_LINT = $(CXXFLAGS_BASE) $(OMPFLAGS) -DPROJECT_NAME=\"$(PROJECT)\" -I$(INC_DIR) $(GTEST_CFLAGS)

# Linker flags
# Base LDLIBS
LDLIBS_BASE = -lpthread
LDAPPFLAGS = $(OMPFLAGS) $(LDLIBS_BASE)
LDGTESTFLAGS = $(OMPFLAGS) $(LDLIBS_BASE) -L/usr/lib/x86_64-linux-gnu -lgtest -lgtest_main

# Source Files
APP_SRC = main.cpp # main.cpp is in the root directory
LIB_SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRC_FILES = $(TEST_DIR)/$(TESTPROJECT).cpp

# Object Files
APP_OBJ = $(BIN_DIR)/$(notdir $(APP_SRC:.cpp=.o))
LIB_OBJ = $(addprefix $(BIN_DIR)/lib/,$(notdir $(LIB_SRC_FILES:.cpp=.o)))
TEST_OBJ_FILE = $(patsubst $(TEST_DIR)/%.cpp,$(BIN_DIR)/test/%.o,$(TEST_SRC_FILES))

# Dependencies
DEPS = $(wildcard $(INC_DIR)/*.h)

# Targets
.PHONY: all test clean cleanall info lint default coverage

default: all

all: info $(PROJECT)

info:
	@echo "Building project $(PROJECT) with OpenMP enabled..."
	@echo "GTest CFLAGS: $(GTEST_CFLAGS)"
	@echo "GTest LIBS: $(GTEST_LIBS)"

# Build application
$(PROJECT): $(APP_OBJ) $(BIN_DIR)/$(LIBPROJECT)
	@echo "Linking application $(PROJECT)..."
	$(CXX) $(CXXFLAGS_COMPILE) -o $@ $(APP_OBJ) $(BIN_DIR)/$(LIBPROJECT) $(LDAPPFLAGS)

# Build library
$(BIN_DIR)/$(LIBPROJECT): $(LIB_OBJ)
	@echo "Creating library $(LIBPROJECT)..."
	@mkdir -p $(dir $@) # Убедимся, что директория для .a существует, если она не в корне
	$(AR) $(ARFLAGS) $@ $^

# Build tests executable
$(TESTPROJECT): $(TEST_OBJ_FILE) $(BIN_DIR)/$(LIBPROJECT)
	@echo "Linking test executable $(TESTPROJECT)..."
	$(CXX) $(CXXFLAGS_COMPILE) -o $@ $(TEST_OBJ_FILE) $(BIN_DIR)/$(LIBPROJECT) $(LDGTESTFLAGS)

# Target to run tests
test: $(TESTPROJECT)
	@echo "Running unit tests for $(TESTPROJECT)..."
	@if [ -f ./$(TESTPROJECT) ]; then \
		./$(TESTPROJECT); \
	else \
		echo "Test executable '$(TESTPROJECT)' not found or not runnable. Ensure '$(TEST_SRC_FILES)' exists and compiles."; \
	fi

# Rule to compile application source file (main.cpp from root)
$(APP_OBJ): $(APP_SRC) $(DEPS)
	@echo "Compiling application source $< -> $@..."
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_COMPILE) -c -o $@ $<

# Rule to compile library source files (from SRC_DIR)
$(BIN_DIR)/lib/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	@echo "Compiling library source $< -> $@..."
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_COMPILE) -c -o $@ $<

# Rule to compile test source file (from TEST_DIR)
$(TEST_OBJ_FILE): $(TEST_SRC_FILES) $(DEPS) $(BIN_DIR)/$(LIBPROJECT)
	@echo "Compiling test source $< -> $@..."
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_COMPILE) -c -o $@ $<

# Clean rules
clean:
	@echo "Cleaning object files..."
	rm -f $(APP_OBJ) 
	rm -f $(BIN_DIR)/lib/*.o 
	rm -f $(TEST_OBJ_FILE) 

cleanall: clean
	@echo "Cleaning executables and library..."
	rm -f $(PROJECT) $(BIN_DIR)/$(LIBPROJECT) $(TESTPROJECT)
	rm -rf $(BIN_DIR) 
	rm -f massif* 
	rm -f test_output_Clockwise.bmp test_output_ContClockwise.bmp test_output_p_Clockwise_parallel.bmp test_output_p_ContClockwise_parallel.bmp temp_test_image.bmp_Clockwise.bmp

lint:
	@echo "Running Clang-Tidy..."
	clang-tidy $(APP_SRC) $(LIB_SRC_FILES) -- $(CXXFLAGS_LINT) -I$(INC_DIR)

coverage: CXXFLAGS = $(CXXFLAGS_COVERAGE)
coverage: $(TESTPROJECT)
	@echo "Running tests for coverage..."
	./$(TESTPROJECT)
	@echo "Generating coverage data..."
	mkdir -p html_coverage
	lcov --ignore-errors version --ignore-errors mismatch --gcov-tool gcov --capture --directory . --output-file html_coverage/coverage.info --rc geninfo_unexecuted_blocks=1 --no-external --base-directory .