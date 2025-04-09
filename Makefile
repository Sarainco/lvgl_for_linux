#
# Makefile
#
#CC 			?= gcc
#CXX 			?= g++
TOOLCHAIN_DIR  := /opt/atk-dlrv1126-toolchain
CC 				= $(TOOLCHAIN_DIR)/bin/arm-linux-gnueabihf-gcc
CXX 			= $(TOOLCHAIN_DIR)/bin/arm-linux-gnueabihf-g++
SYSROOT 		:= $(TOOLCHAIN_DIR)/arm-buildroot-linux-gnueabihf/sysroot
LVGL_DIR_NAME 	= lvgl
LVGL_DIR 		= .
INCLUDE_DIR		= ./include
SRC_DIR			= ./src

WARNINGS		:= -Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith \
					-fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits \
					-Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security \
					-Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body \
					-Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -std=gnu99

CFLAGS 			= -O3 -g0 -ggdb -I$(LVGL_DIR)/ $(WARNINGS)
LDFLAGS 		= -lm
BIN 			= vs800.bin
BUILD_DIR 		= ./build
BUILD_OBJ_DIR 	= $(BUILD_DIR)/obj
BUILD_BIN_DIR 	= $(BUILD_DIR)/bin

prefix 			?= /usr
bindir 			?= $(prefix)/bin

# 链接选项
LDFLAGS := --sysroot=$(SYSROOT) \
           -L$(SYSROOT)/usr/lib \
           -Wl,-rpath-link=$(SYSROOT)/lib:$(SYSROOT)/usr/lib \
           -pthread -ldl

# 库列表
OPENCV_LIBS := opencv_core opencv_imgcodecs opencv_imgproc \
               opencv_features2d opencv_flann opencv_highgui opencv_freetype
RKMEDIA_LIBS := easymedia rga rkaiq rockx sample_common_isp


#Collect the files to compile
MAINSRC          = ./main.c

include $(LVGL_DIR)/lvgl/lvgl.mk
#include $(LVGL_DIR)/lv_100ask_lesson_demos/lv_100ask_lesson_demos.mk
include $(LVGL_DIR)/log/tapah_log.mk

CFLAGS 			+= -DRKAIQ

CFLAGS			+= -I$(LVGL_DIR)
CFLAGS			+= -I$(SYSROOT)/usr/include \
				   -I$(SYSROOT)/usr/include/rga \
				   -I$(SYSROOT)/usr/include/easymedia \
				   -I$(SYSROOT)/usr/include/rkaiq/uAPI \
				   -I$(SYSROOT)/usr/include/rkaiq/xcore \
				   -I$(SYSROOT)/usr/include/rkaiq/algos \
				   -I$(SYSROOT)/usr/include/rkaiq/common \
				   -I$(SYSROOT)/usr/include/rkaiq/iq_parser \
				   -I$(SYSROOT)/usr/include/rockx
CFLAGS			+= -I$(INCLUDE_DIR)/lv_port
#CFLAGS			+= -I$(INCLUDE_DIR)/vi_vo
CFLAGS			+= -I$(INCLUDE_DIR)/rkmedia

CSRCS 			+= $(wildcard $(SRC_DIR)/lv_port/*.c)
CSRCS 			+= $(wildcard $(SRC_DIR)/input_event/*.c)
#CSRCS			+= $(wildcard $(SRC_DIR)/vi_vo/*.c)
CSRCS 			+= $(wildcard $(SRC_DIR)/rkmedia/*.c)


OBJEXT 			?= .o

AOBJS 			= $(ASRCS:.S=$(OBJEXT))
COBJS 			= $(CSRCS:.c=$(OBJEXT))
CXXOBJS 		= $(CXXSRCS:.cpp=$(OBJEXT))

MAINOBJ 		= $(MAINSRC:.c=$(OBJEXT))

SRCS 			= $(ASRCS) $(CSRCS) $(CXXSRCS) $(MAINSRC)
OBJS 			= $(AOBJS) $(COBJS) $(CXXOBJS) $(MAINOBJ)
TARGET 			= $(addprefix $(BUILD_OBJ_DIR)/, $(patsubst ./%, %, $(OBJS)))

## MAINOBJ -> OBJFILES


all: default

$(BUILD_OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX)  $(CXXFLAGS) -c $< -o $@
	@echo "CXX $<"

$(BUILD_OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"

$(BUILD_OBJ_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"


default: $(TARGET)
	@mkdir -p $(dir $(BUILD_BIN_DIR)/)
	$(CXX) -o $(BUILD_BIN_DIR)/$(BIN) $(TARGET) $(LDFLAGS) $(addprefix -l,$(OPENCV_LIBS)) $(addprefix -l,$(RKMEDIA_LIBS))

clean:
	rm -rf $(BUILD_DIR)

install:
	install -d $(DESTDIR)$(bindir)
	install $(BUILD_BIN_DIR)/$(BIN) $(DESTDIR)$(bindir)

uninstall:
	$(RM) -r $(addprefix $(DESTDIR)$(bindir)/,$(BIN))
