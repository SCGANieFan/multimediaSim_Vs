include toolchain.mk

# ===================== 变量初始化 =====================
ALL_OBJS :=
ALL_SRCS :=

# ===================== Verbose 开关 =====================
# make V=1 开启详细打印
V ?=
ifeq ($(strip $(V)),)
  Q := @                    # 静默模式：用@前缀隐藏命令
  quiet := $(1)             # 替换成简洁信息
else
  Q :=                      # 详细模式：不用@，让make打印命令
  quiet := @echo $(1)       # 额外打印信息
endif

# ===================== 辅助函数 =====================

# 清理路径双斜杠 // → /
define clean-path
$(strip $(subst //,/,$(1)))
endef

# 自动查找 Makefile / makefile
define find_makefile
    $(call clean-path,\
        $(if $(wildcard $(1)/Makefile),$(1)/Makefile,\
        $(if $(wildcard $(1)/makefile),$(1)/makefile,\
        ))\
    )
endef

# ===================== 方案B核心：深度优先遍历（自动捕获COMPILE_FLAGS，不改动子makefile） =====================
# traverse_inner：辅助函数 — 用位置参数封装单次遍历，避免递归污染
#   $(1)：本层待遍历的 COMPILE_SRCS（已展开为文本列表）
#   $(2)：本层目录路径（已展开为文本）
#   $(3)：本层累积的编译选项（已展开为文本）
# 关键：这三个都是位置参数（文本替换），不会被递归调用中的任何赋值修改
define traverse_inner
    $(foreach item,$(1),
        $(if $(filter %.o,$(item)),
            $(call collect_obj,$(2),$(item),$(3)),
            $(call traverse_dir,$(call clean-path,$(2)/$(item)),$(3))
        )
    )
endef

# traverse_dir：主函数 — 读取指定目录的 makefile，收集 .o 或递归进入子目录
#   $(1)：当前目录（相对项目根）
#   $(2)：父目录累积的专属编译选项（用于继承）
define traverse_dir
    $(eval __SAVED_CUR_DIR   := $(CUR_DIR))
    $(eval __SAVED_SRCS     := $(COMPILE_SRCS))
    $(eval __SAVED_DIR_FLAGS := $(DIR_FLAGS))
    $(eval __SAVED_ADDED    := $(DIR_ADDED_FLAGS))
    $(eval __SAVED_PARENT   := $(PARENT_FLG))
    $(eval __SAVED_MK       := $(MK))

    $(eval __DIR := $(call clean-path,$(1)))
    $(eval __PARENT_FLG := $(2))
    $(eval __MK := $(call find_makefile,$(__DIR)))

    $(info === 扫描: $(__DIR))

    $(if $(__MK),
        $(eval __SAVED_FLAGS := $(COMPILE_FLAGS))
        $(eval MAKEFILE_LIST := $(MAKEFILE_LIST) $(__MK))
        $(eval include $(__MK))
        $(eval __LOCAL_SRCS := $(COMPILE_SRCS))
        $(eval __DIR_ADDED_FLAGS := $(filter-out $(__SAVED_FLAGS),$(COMPILE_FLAGS)))
        $(eval COMPILE_FLAGS := $(__SAVED_FLAGS))
        $(eval __DIR_FLAGS := $(__PARENT_FLG) $(__DIR_ADDED_FLAGS))
        $(call traverse_inner,$(__LOCAL_SRCS),$(__DIR),$(__DIR_FLAGS))
        ,
        $(info 跳过: $(__DIR) 无Makefile)
    )

    $(eval CUR_DIR := $(__SAVED_CUR_DIR))
    $(eval COMPILE_SRCS := $(__SAVED_SRCS))
    $(eval DIR_FLAGS := $(__SAVED_DIR_FLAGS))
    $(eval DIR_ADDED_FLAGS := $(__SAVED_ADDED))
    $(eval PARENT_FLG := $(__SAVED_PARENT))
    $(eval MK := $(__SAVED_MK))
endef

# ===================== 收集目标文件 + 绑定目标特定变量 =====================
# 参数1：源文件所在目录
# 参数2：.o 文件名
# 参数3：该目录及所有父目录累积的专属编译选项
define collect_obj
    $(eval DIR     := $(call clean-path,$(1)))
    $(eval OBJNAME := $(2))
    $(eval OBJ     := $(call clean-path,$(OBJ_DIR)/$(DIR)/$(OBJNAME)))
    $(eval SRC_C   := $(call clean-path,$(DIR)/$(OBJNAME:.o=.c)))
    $(eval SRC_CPP := $(call clean-path,$(DIR)/$(OBJNAME:.o=.cpp)))
    $(eval SRC_S   := $(call clean-path,$(DIR)/$(OBJNAME:.o=.s)))
    $(eval DIR_FLG := $(3))

    $(eval src := )
    $(if $(wildcard $(SRC_C)),$(eval src := $(SRC_C)))
    $(if $(wildcard $(SRC_CPP)),$(eval src := $(SRC_CPP)))
    $(if $(wildcard $(SRC_S)),$(eval src := $(SRC_S)))

    $(if $(src),
        $(eval ALL_SRCS += $(src))
        $(eval ALL_OBJS += $(OBJ))
        $(eval $(OBJ): COMPILE_FLAGS = $(COMPILE_FLAGS) $(DIR_FLG))
    )
endef

# ===================== 模式规则（V=1 开启详细打印） =====================

# 规则1：.c 编译成 .o
$(OBJ_DIR)/%.o: %.c
	$(Q)$(MKDIR) $(dir $@)
# 	$(if $(strip $(V)),@echo "  [CC] $< -> $@"$(info       cmd: $(CC) $(COMPILE_FLAGS) -c $< -o $@),@echo " [CC] $<")
	$(Q)$(CC) $(COMPILE_FLAGS) -c $< -o $@

# 规则2：.cpp 编译成 .o
$(OBJ_DIR)/%.o: %.cpp
	$(Q)$(MKDIR) $(dir $@)
# 	$(if $(strip $(V)),@echo " [CXX] $< -> $@"$(info       cmd: $(CXX) $(COMPILE_FLAGS) -c $< -o $@),@echo " [CXX] $<")
	$(Q)$(CXX) $(COMPILE_FLAGS) -c $< -o $@

# 规则3：.s 汇编成 .o
$(OBJ_DIR)/%.o: %.s
	$(Q)$(MKDIR) $(dir $@)
# 	$(if $(strip $(V)),@echo "  [AS] $< -> $@"$(info       cmd: $(AS) $(AS_FLAGS) -c $< -o $@),@echo " [AS] $<")
	$(Q)$(AS) $(AS_FLAGS) -c $< -o $@