CSRCS += $(shell find -L $(LVGL_DIR)/bsp/debug -name "*.c")
CSRCS += $(shell find -L $(LVGL_DIR)/bsp/file -name "*.c")
CSRCS += $(shell find -L $(LVGL_DIR)/bsp/i2c -name "*.c")