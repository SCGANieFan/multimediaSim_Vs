# ==========================================================
# 平台选择：arm (嵌入式) 或 linux (本机开发测试)
# 用法：make PLATFORM=arm  或  make PLATFORM=linux
# ==========================================================
PLATFORM ?= arm

# ----------------------------------------------------------
# Linux 本机：gcc/g++（用于开发、gdb 调试、valgrind 测试）
# ----------------------------------------------------------
ifeq ($(PLATFORM),linux)
CC      := gcc
CXX     := g++
AS      := gcc -x assembler-with-cpp
LD      := ld
OBJCOPY := objcopy
SIZE    := size

COMPILE_FLAGS := -Wall -Wextra -O2 \
                 -ffunction-sections -fdata-sections \
                 -g -ggdb3 \
                 -m32  # 编译为 32 位，与 ARM 平台一致，避免 void*->uint32_t 截断错误

CXX_FLAGS := $(COMPILE_FLAGS) -std=c++17 -fexceptions -frtti
AS_FLAGS  := $(COMPILE_FLAGS)

LINK_FLAGS := -m32 -Wl,--gc-sections
SUFFIX     := _linux
PLATFORM_DEF := -DSMf_PLATFORM_LINUX
endif

# ----------------------------------------------------------
# ARM 嵌入式：arm-none-eabi-gcc/g++（最终目标）
# ----------------------------------------------------------
ifeq ($(PLATFORM),arm)
CROSS_COMPILE := arm-none-eabi-
CC            := $(CROSS_COMPILE)gcc
CXX           := $(CROSS_COMPILE)g++
AS            := $(CROSS_COMPILE)gcc -x assembler-with-cpp
LD            := $(CROSS_COMPILE)ld
OBJCOPY       := $(CROSS_COMPILE)objcopy
SIZE          := $(CROSS_COMPILE)size

COMPILE_FLAGS := -Wall -Wextra -O2 \
                 -mcpu=cortex-m4 -mthumb \
                 -ffunction-sections -fdata-sections

CXX_FLAGS := $(COMPILE_FLAGS) -std=c++17 -fno-exceptions -fno-rtti
AS_FLAGS  := $(COMPILE_FLAGS)

LINK_FLAGS := -Wl,--gc-sections -lm -lc
SUFFIX     :=
PLATFORM_DEF := -DSMf_PLATFORM_ARM
endif

# 把平台宏加到编译选项
COMPILE_FLAGS += $(PLATFORM_DEF)

# 输出目录：按平台区分 build/obj 和 build/bin
BUILD_DIR := build/$(PLATFORM)
BIN_DIR   := $(BUILD_DIR)/bin
OBJ_DIR   := $(BUILD_DIR)/obj
TARGET    := $(BIN_DIR)/app.elf

MKDIR := mkdir -p
RM    := rm -rf