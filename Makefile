ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/base_rules

TARGET 				:= payload
BLVERSION_MAJOR		:= 0
BLVERSION_MINOR		:= 3
BUILD_VER			:= 124

BUILD 				:= build
OUTPUT 				:= output
SOURCEDIR 			:= src
DATA				:= data
SOURCES		      	:= src
INCLUDES			:= include src


$(shell mkdir -p $(BUILD))
$(shell mkdir -p $(OUTPUT))

#Get SOURCE Subdirs
SOURCES	+=	$(foreach dir,$(SOURCES)/,$(shell find $(dir) -maxdepth 10 -type d))
#$(error $(SOURCES))

VPATH = $(dir  ./$(SOURCEDIR)/) $(dir $(wildcard ./$(SOURCEDIR)/*/)) $(dir $(wildcard ./$(SOURCEDIR)/*/*/))
CFILES			:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
SFILES			:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES		:=  $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

OFILES_BIN		:= $(addsuffix .o,$(BINFILES))
OFILES_SRC		:= $(SFILES:.s=.o) $(CFILES:.c=.o)
HFILES_BIN		:= $(addsuffix .h,$(subst .,_,$(BINFILES)))

OBJS 					= $(addprefix $(BUILD)/, $(OFILES_BIN) $(OFILES_SRC))


INCLUDE				:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
										$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
										-I$(BUILD)

CUSTOMDEFINES := -DLOAD_BUILD_VER=$(BUILD_VER)

ARCH := -march=armv4t -mtune=arm7tdmi -mthumb -mthumb-interwork
CFLAGS = $(INCLUDE) $(ARCH) -O2 -nostdlib -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-inline -std=gnu11 -Wall $(CUSTOMDEFINES)
LDFLAGS = $(ARCH) -nostartfiles -lgcc -Wl,--nmagic,--gc-sections


.PHONY: all clean

all: directories $(OUTPUT)/$(TARGET).bin
	@echo $(HFILES_BIN)
	@echo -n "Payload size is "
	@wc -c < $(OUTPUT)/$(TARGET).bin
	@echo "Max size is 126296 Bytes."

directories:
	@mkdir -p "$(BUILD)"
	@mkdir -p "$(OUTPUT)"
#	$(error "i am here")
	
clean:
	@rm -rf $(OBJS)
	@rm -rf $(BUILD)
	@rm -rf $(OUTPUT)
	@rm -rf logo_bmp.h

$(MODULEDIRS):
	@mkdir -p "$(BUILD)"
	@mkdir -p "$(OUTPUT)"
	$(MAKE) -C $@ $(MAKECMDGOALS)


$(OUTPUT)/$(TARGET).bin: $(BUILD)/$(TARGET).elf $(MODULEDIRS)
	$(OBJCOPY) -S -O binary $< $@
	@printf STDV$(BUILD_VER) >> $@

$(TARGET).bin: $(BUILD)/$(TARGET).elf $(MODULEDIRS)
	$(OBJCOPY) -S -O binary $< $(OUTPUT)/$@
	@printf STDV$(BUILD_VER) >> $(OUTPUT)/$@

$(BUILD)/$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) -T $(SOURCEDIR)/link.ld $^ -o $@

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/$(TARGET):
$(OFILES_SRC)	: $(HFILES_BIN)

$(BUILD)/%.bmp.o %_bmp.h:	data/%.bmp
	@echo $(notdir $<)
	@$(bin2o)	
	