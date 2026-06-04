CUR_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

COMPILE_SRCS := \
    framework/ \
#     framework/ \
#     dbg/ \
#     io/ \
#     misc/ \
#     msg/ \

export SMF_INC := $(CUR_DIR)inc/

COMPILE_FLAGS += \
	-I$(SMF_INC)3th \
	-I$(SMF_INC)algo \
	-I$(SMF_INC)apps \
	-I$(SMF_INC)com \
	-I$(SMF_INC)eles \
	-I$(SMF_INC)hpp \
	-I$(CUR_DIR)porting/inc
