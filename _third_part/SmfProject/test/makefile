CUR_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
OBJ_CPP := $(patsubst $(CUR_DIR)%,%,$(wildcard $(CUR_DIR)*.cpp))
OBJ_C := $(patsubst $(CUR_DIR)%,%,$(wildcard $(CUR_DIR)*.c))
OBJ_S := $(patsubst $(CUR_DIR)%,%,$(wildcard $(CUR_DIR)*.s))

COMPILE_SRCS := $(OBJ_C:.c=.o) $(OBJ_S:.S=.o) $(OBJ_CPP:.cpp=.o)

# COMPILE_SRCS := \
#     framework/ \
#     framework/ \
#     dbg/ \
#     io/ \
#     misc/ \
#     msg/ \

export SMF_INC := $(CUR_DIR)../Smf/inc/

COMPILE_FLAGS += \
	-I$(SMF_INC)3th


# COMPILE_FLAGS += \
# 	-I$(SMF_INC)3th \
# 	-I$(SMF_INC)algo \
# 	-I$(SMF_INC)apps \
# 	-I$(SMF_INC)com \
# 	-I$(SMF_INC)eles \
# 	-I$(SMF_INC)hpp \
# 	-I$(CUR_DIR)porting/inc

COMPILE_FLAGS += \
	-Wno-unused-parameter \
	-Wno-strict-aliasing






