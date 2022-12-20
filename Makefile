ifndef DEBUG
DEBUG = 0
endif

ifndef USE_FREEIMAGE
USE_FREEIMAGE = 0
endif

ifneq ($(shell grep -c -i "ID=nixos" /etc/os-release),0)
FT2_CFLAGS := -I$(shell dirname $(shell find /nix/store/ -name ft2build.h | head -qn1))
FT2_LDFLAGS := -L$(shell dirname $(shell find /nix/store/ -name libfreetype.a | head -qn1)) -lfreetype
else
FT2_CFLAGS := $(shell pkg-config --cflags freetype2)
FT2_LDFLAGS := $(shell pkg-config --libs freetype2)
endif

OPTCFLAGS := -O3 -march=native
DBG1FLAGS := -g
DBG2FLAGS := -g -fsanitize=address

CC := gcc -fdiagnostics-color=always
CFLAGS := -D__NO_INLINE__ -w -Wno-unused-parameter -std=c17 $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf zlib gtk+-3.0) $(FT2_CFLAGS)
LDFLAGS := -Wl,--copy-dt-needed-entries
LDLIBS := $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf zlib gtk+-3.0) $(FT2_LDFLAGS) -lm

ifeq ($(DEBUG),)
DEBUG := 0
endif

ifeq ($(DEBUG),0)
CFLAGS := $(CFLAGS) $(OPTCFLAGS)
LDFLAGS := $(LDFLAGS) $(OPTCFLAGS)
endif

ifeq ($(DEBUG),1)
CFLAGS := $(CFLAGS) $(DBG1FLAGS)
LDFLAGS := $(LDFLAGS) $(DBG1FLAGS)
endif

ifeq ($(DEBUG),2)
CFLAGS := $(CFLAGS) $(DBG2FLAGS)
LDFLAGS := $(LDFLAGS) $(DBG2FLAGS)
endif

FI_CFLAGS := -DUSE_FREEIMAGE
FI_LDLIBS := -lfreeimage

ifneq ($(USE_FREEIMAGE),0)
CFLAGS := $(CFLAGS) $(FI_CFLAGS)
LDLIBS := $(LDLIBS) $(FI_LDLIBS)
endif

SRC_DIR := src
OBJ_DIR := obj
BUILD_DIR := build
TESTS_DIR := tests

SRC_MAINS := $(SRC_DIR)/main.c $(SRC_DIR)/solver.c
OBJ_MAINS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_MAINS))

OUTPUT := solver

INCLUDES := -I$(SRC_DIR)

HDR_FILES := $(shell find $(SRC_DIR)/ -type f -name '*.h')

SRC_FILES := $(shell find $(SRC_DIR)/ -type f -name '*.c')
SRC_FILES := $(filter-out $(SRC_MAINS),$(SRC_FILES))

OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

all: reynet solver

docs: doxygen/doxygen.conf $(HDR_FILES)
	doxygen doxygen/doxygen.conf

solver: $(BUILD_DIR) $(OBJ_DIR) $(OBJ_FILES) $(OBJ_DIR)/solver.o
	$(CC) $(LDFLAGS) $(OBJ_FILES) $(OBJ_DIR)/solver.o $(LDLIBS) -o $(BUILD_DIR)/$@

reynet: $(BUILD_DIR) $(OBJ_DIR) $(OBJ_FILES) $(OBJ_DIR)/main.o $(SRC_DIR)/app/gui_main.h
	$(CC) $(LDFLAGS) $(OBJ_FILES) $(OBJ_DIR)/main.o $(LDLIBS) -o $(BUILD_DIR)/$@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BUILD_DIR)


# Tests

testsdirs:
	mkdir -p $(TESTS_DIR)/out
	mkdir -p $(TESTS_DIR)/out/grids
	mkdir -p $(TESTS_DIR)/out/cells
	mkdir -p $(TESTS_DIR)/out/draw_grid

build/%: $(TESTS_DIR)/%.c $(BUILD_DIR) $(OBJ_DIR) $(OBJ_FILES)
	@mkdir -p $(OBJ_DIR)/$(TESTS_DIR)
	@mkdir -p $(BUILD_DIR)/$(TESTS_DIR)
	$(eval OBJ_TEST := $(patsubst %.c,$(OBJ_DIR)/%.o,$<))
	$(eval OUT_TEST := $(patsubst build/%,$(BUILD_DIR)/$(TESTS_DIR)/%,$@))
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $(OBJ_TEST)
	$(CC) $(LDFLAGS) $(OBJ_FILES) $(OBJ_TEST) $(LDLIBS) -o $(OUT_TEST)

run/%: testsdirs
	$(eval OUT_TEST := $(patsubst run/%,$(BUILD_DIR)/$(TESTS_DIR)/%,$@))
	./$(OUT_TEST) $(ARGS)

test/%: build/% testsdirs
	$(eval OUT_TEST := $(patsubst test/%,$(BUILD_DIR)/$(TESTS_DIR)/%,$@))
	./$(OUT_TEST) $(ARGS)
