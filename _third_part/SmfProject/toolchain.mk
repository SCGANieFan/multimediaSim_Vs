CROSS_COMPILE := arm-none-eabi-
CC            := $(CROSS_COMPILE)gcc
CXX           := $(CROSS_COMPILE)g++
AS            := $(CROSS_COMPILE)gcc -x assembler-with-cpp
LD            := $(CROSS_COMPILE)ld
OBJCOPY       := $(CROSS_COMPILE)objcopy
SIZE          := $(CROSS_COMPILE)size

# 全局编译参数
COMPILE_FLAGS := -Wall -Wextra -O2 \
                 -mcpu=cortex-m4 -mthumb \
                 -ffunction-sections -fdata-sections

CXX_FLAGS := $(COMPILE_FLAGS) -std=c++17 -fno-exceptions -fno-rtti
AS_FLAGS  := $(COMPILE_FLAGS)

BUILD_DIR := build
BIN_DIR   := $(BUILD_DIR)/bin
OBJ_DIR   := $(BUILD_DIR)/obj
TARGET    := $(BIN_DIR)/app.elf

MKDIR := mkdir -p
RM    := rm -rf