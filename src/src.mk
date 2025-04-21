CSRCS += $(shell find -L $(LVGL_DIR)/src/input_event -name "*.c")
CSRCS += $(shell find -L $(LVGL_DIR)/src/rkmedia -name "*.c")
CSRCS += $(shell find -L $(LVGL_DIR)/src/dpd60 -name "*.c")