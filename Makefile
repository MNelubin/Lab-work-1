# Configuration
PROJECT = lgo
LIBPROJECT = $(PROJECT).a
TESTPROJECT = test-$(PROJECT)

# Directories
SRC_DIR = src
TEST_DIR = tests
INC_DIR = include
BIN_DIR = out
COVERAGE_BUILD = 0

# Tools
CXX = g++
AR = ar
ARFLAGS = rcs


GTEST_CFLAGS ?= $(shell pkg-config --cflags gtest 2>/dev/null)
GTEST_LIBS   ?= $(shell pkg-config --libs gtest 2>/dev/null)

# Flags
# Base CXXFLAGS
CXXFLAGS_BASE = -I$(INC_DIR) -std=c++17 -Wall -g -fPIC -Werror -Wpedantic
# OpenMP flags
OMPFLAGS = -fopenmp -O3 

# Stamp file to track COVERAGE_BUILD state for object files
OBJ_STATE_DIR = $(BIN_DIR)/.obj_state
COVERAGE_TAG = $(if $(filter 1,$(COVERAGE_BUILD)),coverage,nocoverage)
OBJ_STATE_STAMP = $(OBJ_STATE_DIR)/$(COVERAGE_TAG).stamp

# Application specific CXXFLAGS
CXXFLAGS_COMPILE = $(CXXFLAGS_BASE) $(OMPFLAGS) -DPROJECT_NAME="$(PROJECT)" -isystem /usr/lib/gcc/x86_64-linux-gnu/13/include $(GTEST_CFLAGS)
ifeq ($(COVERAGE_BUILD),1)
  CXXFLAGS_COMPILE += -fprofile-arcs -ftest-coverage
endif
CXXFLAGS_LINT = $(CXXFLAGS_BASE) $(OMPFLAGS) -DPROJECT_NAME=\"$(PROJECT)\" -I$(INC_DIR) $(GTEST_CFLAGS)

# Linker flags
# Base LDLIBS
LDLIBS_BASE = -lpthread
LDAPPFLAGS = $(OMPFLAGS) $(LDLIBS_BASE)
ifeq ($(COVERAGE_BUILD),1)
  LDAPPFLAGS += -fprofile-arcs -ftest-coverage # These are also linker flags
endif
LDGTESTFLAGS = $(OMPFLAGS) $(LDLIBS_BASE) -L/usr/lib/x86_64-linux-gnu -lgtest -lgtest_main
ifeq ($(COVERAGE_BUILD),1)
  LDGTESTFLAGS += -fprofile-arcs -ftest-coverage -lgcov
endif

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
	$(CXX) $(CXXFLAGS_COMPILE) -o $@ $(APP_OBJ) $(BIN_DIR)/$(LIBPROJECT) $(LDAPPFLAGS) $(if $(filter 1,$(COVERAGE_BUILD)),-L/usr/lib/gcc/x86_64-linux-gnu/13/ -lgcov)

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

# Rule to create the stamp file
$(OBJ_STATE_STAMP):
	@mkdir -p $(OBJ_STATE_DIR)
	@rm -f $(OBJ_STATE_DIR)/*.stamp # Remove any other state stamp
	@touch $@

# Rule to compile application source file (main.cpp from root)
$(APP_OBJ): $(APP_SRC) $(DEPS) $(OBJ_STATE_STAMP)
	@echo "Compiling application source $< -> $@ (Coverage: $(COVERAGE_TAG))..."
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_COMPILE) -c -o $@ $<

# Rule to compile library source files (from SRC_DIR)
$(BIN_DIR)/lib/%.o: $(SRC_DIR)/%.cpp $(DEPS) $(OBJ_STATE_STAMP)
	@echo "Compiling library source $< -> $@ (Coverage: $(COVERAGE_TAG))..."
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_COMPILE) -c -o $@ $<

# Rule to compile test source file (from TEST_DIR)
$(TEST_OBJ_FILE): $(TEST_SRC_FILES) $(DEPS) $(BIN_DIR)/$(LIBPROJECT) $(OBJ_STATE_STAMP)
	@echo "Compiling test source $< -> $@ (Coverage: $(COVERAGE_TAG))..."
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
	rm -f *.gcno *.gcda
	@echo "Cleaning build state files..."
	rm -rf $(OBJ_STATE_DIR)
	rm -rf html_coverage/

lint:
	@echo "Running Clang-Tidy..."
	clang-tidy $(APP_SRC) $(LIB_SRC_FILES) -- $(CXXFLAGS_LINT) -I$(INC_DIR)

coverage:
	@echo "Cleaning old coverage data and object files..."
	rm -f *.gcno *.gcda
	rm -f $(BIN_DIR)/lib/*.o
	rm -f $(TEST_OBJ_FILE)
	rm -f $(BIN_DIR)/$(LIBPROJECT)
	rm -f $(TESTPROJECT)
	@echo "Building with coverage flags..."
	$(MAKE) cleanall
	$(MAKE) $(TESTPROJECT) COVERAGE_BUILD=1
	@echo "Running tests for coverage..."
	./$(TESTPROJECT)
	@echo "Generating coverage data..."
	mkdir -p html_coverage
	lcov --ignore-errors mismatch,mismatch  --no-markers --gcov-tool gcov-13 --capture --directory out/lib --directory out/test --output-file html_coverage/coverage.info --rc geninfo_unexecuted_blocks=1 --rc lcov_branch_coverage=1 --rc lcov_function_coverage=1 --rc geninfo_auto_base=1 --rc geninfo_skip_unreachable_blocks=1 --no-external --base-directory .