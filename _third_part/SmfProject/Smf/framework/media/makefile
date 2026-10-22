CUR_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
OBJ_CPP := $(patsubst $(CUR_DIR)%,%,$(wildcard $(CUR_DIR)*.cpp))
OBJ_C := $(patsubst $(CUR_DIR)%,%,$(wildcard $(CUR_DIR)*.c))
OBJ_S := $(patsubst $(CUR_DIR)%,%,$(wildcard $(CUR_DIR)*.s))

COMPILE_SRCS := $(OBJ_C:.c=.o) $(OBJ_S:.S=.o) $(OBJ_CPP:.cpp=.o)

COMPILE_SRCS := \
    audio/ \
    video/ 

COMPILE_FLAGS += \
    -I$(CUR_DIR)audio \
    -I$(CUR_DIR)video 

COMPILE_FLAGS += \
	-Wno-sign-compare 
#     -Wno-unused-parameter \
# 	-Wno-strict-aliasing \



