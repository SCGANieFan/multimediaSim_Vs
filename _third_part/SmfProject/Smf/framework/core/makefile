CUR_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

COMPILE_SRCS := \
    base/ \
    container/ \
    dbg/ \
    io/ \
    misc/ \
    plugin/ \
    pool/ \
    script/ \
    shm/ \
    std/ \
    meida/ \
    priv/ \
    priv/

COMPILE_FLAGS += \
    -I$(CUR_DIR)base \
    -I$(CUR_DIR)container \
    -I$(CUR_DIR)dbg \
    -I$(CUR_DIR)io \
    -I$(CUR_DIR)misc \
    -I$(CUR_DIR)msg \
    -I$(CUR_DIR)plugin \
    -I$(CUR_DIR)pool \
    -I$(CUR_DIR)script \
    -I$(CUR_DIR)shm \
    -I$(CUR_DIR)src \
    -I$(CUR_DIR)std

COMPILE_FLAGS += \
    -Wno-unused-parameter \
	-Wno-strict-aliasing \
	-Wno-sign-compare \
    -Wno-cast-function-type \
    -Wno-nonnull \
    -Wno-missing-field-initializers


    