include traverse.mk

# ===================== 配置 =====================
START_DIRS := Smf

# ===================== 关键：解析期立即遍历，收集 ALL_OBJS =====================
# 这一步在 Make 读取文件时就完成，得到所有需要的 .o 文件列表
$(foreach dir,$(START_DIRS),$(call traverse_dir,$(dir)))

# V=1 时打印汇总信息
ifeq ($(strip $(V)),1)
    $(info )
    $(info ==== 扫描完成 ====)
    $(info 源文件 ($(words $(ALL_SRCS)) 个):)
    $(foreach s,$(ALL_SRCS),$(info   $(s)))
    $(info )
    $(info 目标文件 ($(words $(ALL_OBJS)) 个):)
    $(foreach o,$(ALL_OBJS),$(info   $(o)))
    $(info )
endif

# ===================== 目标定义 =====================

# 最终目标：app.elf 依赖所有 .o 文件
# Make 会自动通过模式规则编译那些过期的 .o
all: prepare check $(TARGET) done

prepare:
	@$(MKDIR) $(OBJ_DIR) $(BIN_DIR)

check: prepare
	@if [ -z "$(ALL_SRCS)" ]; then \
		echo "ERROR: 未找到任何源文件，编译退出！"; \
		exit 1; \
	fi

# ✅ 关键改进：$(TARGET) 真正依赖所有 $(ALL_OBJS)
# Make 看到这个依赖，会自动检查每个 .o 是否过期
$(TARGET): $(ALL_OBJS)
	@echo " [LD] $(TARGET)"
# 	$(if $(strip $(V)),@echo "     cmd: $(CC) $(ALL_OBJS) -o $(TARGET) -Wl,--gc-sections -lm -lc")
	@$(CC) $(ALL_OBJS) -o $(TARGET) -Wl,--gc-sections -lm -lc

done: $(TARGET)
	@echo "====================================="
	@echo " 编译完成！"
	@$(SIZE) $(TARGET)

clean:
	@$(RM) $(BUILD_DIR)
	@echo " 清理完成！"

.PHONY: all prepare check done clean
