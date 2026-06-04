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
# 参数1：当前目录
# 参数2：父目录累积的专属编译选项（用于继承）
define traverse_dir
    $(eval CUR_DIR := $(call clean-path,$(1)))
    $(eval PARENT_FLG := $(2))
    $(eval MK := $(call find_makefile,$(CUR_DIR)))

    $(info === 扫描: $(CUR_DIR))

    $(if $(MK),
        $(eval SAVED_FLAGS := $(COMPILE_FLAGS))
        $(eval include $(MK))
        $(eval DIR_ADDED_FLAGS := $(filter-out $(SAVED_FLAGS),$(COMPILE_FLAGS)))
        $(eval COMPILE_FLAGS := $(SAVED_FLAGS))
        $(eval DIR_FLAGS := $(PARENT_FLG) $(DIR_ADDED_FLAGS))
        $(foreach item,$(COMPILE_SRCS),
            $(if $(filter %.o,$(item)),
                $(call collect_obj,$(CUR_DIR),$(item),$(DIR_FLAGS)),
                $(call traverse_dir,$(call clean-path,$(CUR_DIR)/$(item)),$(DIR_FLAGS))
            )
        )
        ,
        $(info 跳过: $(CUR_DIR) 无Makefile)
    )
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
