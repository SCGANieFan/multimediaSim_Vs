CUR_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

COMPILE_SRCS := \
    core/ \
    media/
#     container/ \
#     dbg/ \
#     io/ \
#     misc/ \
#     msg/ \

COMPILE_FLAGS += \
    -I$(CUR_DIR)core \
    -I$(CUR_DIR)core/base \
    -I$(CUR_DIR)core/std \
    -I$(CUR_DIR)core/container \
    -I$(CUR_DIR)media \
    -I$(CUR_DIR)priv
