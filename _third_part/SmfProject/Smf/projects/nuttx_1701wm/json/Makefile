cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))

# obj-y += $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)
obj-y := json.o

#ifeq ($(SMF_CORE), a55)
#obj-y := json_a55.o
#$(shell touch $(srctree)/$(path_projects)/json/json_a55.S)
# $(shell cp $(srctree)/$(path_projects)/json/smf_a55.json . -f)
#$(shell cp $(srctree)/$(path_projects)/json/smf.json . -f)
#endif

#ifeq ($(SMF_CORE), m55c0)
#$(shell touch $(srctree)/$(path_projects)/json/json_a55.S)
#$(shell cp $(srctree)/$(path_projects)/json/smf_a55.json . -f)
#obj-y := json_m55.o
#endif

# ifeq ($(SMF_CORE), m55c0)
# obj-y := json.o
# endif
