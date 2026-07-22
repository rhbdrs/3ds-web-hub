TARGET		:= 3ds_web_hub
BUILD		:= build
SOURCES		:= .
INCLUDES	:= .
CFLAGS		:= -Wall -O2 -g
CXXFLAGS	:= $(CFLAGS)
ASFLAGS		:= -g
LDFLAGS		:= -specs=3dsx.specs
LIBS		:= -lctru -lm

include $(DEVKITPRO)/libctru/switch_rules

.PHONY: all clean
all: $(BUILD)/$(TARGET).3dsx
$(BUILD)/$(TARGET).3dsx: $(BUILD)/$(TARGET).elf
