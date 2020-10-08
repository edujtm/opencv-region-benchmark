CXX      := -g++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror 
BUILD    := ./build
BENCH    := ./benchmark

OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
BENCH_DIR := $(BUILD)/benchmark

TARGET   := regions
INCLUDE  := -Iinclude/

SRC      :=                      \
   $(wildcard src/*.cpp)         \

# All the source files for the different benchmarks
BENCHMARK_SRCS :=                \
   $(wildcard benchmark/*.cpp)   \

BENCHMARK_TARGETS := $(BENCHMARK_SRCS:%.cpp=$(BUILD)/%)

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

# OpenCV 4.4.0 sources and headers
OPENCV_DIR := /home/edujtm/Code/local/OpenCV-4.4.0/opencv/build/installation
OPENCV_FLAGS := `pkg-config --cflags --libs $(OPENCV_DIR)/lib/pkgconfig/opencv4.pc`

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ $(LDFLAGS) $(OPENCV_FLAGS)

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS) $(OPENCV_FLAGS)

$(BENCHMARK_TARGETS): $(BENCHMARK_SRCS) 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(BENCH)/$(notdir $@).cpp -o $@ $(LDFLAGS) $(OPENCV_FLAGS)

.PHONY: all build clean debug release benchmark

# this will compile source code just in case there's dependencies
benchmark: CXXFLAGS += -O2
benchmark: build $(BENCHMARK_TARGETS)

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
	-@rm -rvf $(BENCH_DIR)/*
