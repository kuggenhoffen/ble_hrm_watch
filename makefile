PROJECT_NAME := ble_hrm_watch

NRF_SDK_PATH := /home/jaska/Development/nRF51_SDK_7.1.0
U8G_SRC_PATH := /home/jaska/Development/u8glib_arm/src
U8G_FONT_PATH := /home/jaska/Development/u8glib/sfntsrc

export OUTPUT_FILENAME
#MAKEFILE_NAME := $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
MAKEFILE_NAME := $(MAKEFILE_LIST)
MAKEFILE_DIR := $(dir $(MAKEFILE_NAME) ) 


TEMPLATE_PATH = $(NRF_SDK_PATH)/components/toolchain/gcc
ifeq ($(OS),Windows_NT)
include $(TEMPLATE_PATH)/Makefile.windows
else
include $(TEMPLATE_PATH)/Makefile.posix
endif

MK := mkdir
RM := rm -rf

#echo suspend
ifeq ("$(VERBOSE)","1")
NO_ECHO := 
else
NO_ECHO := @
endif

# Toolchain commands
CC       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc"
AS       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as"
AR       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar" -r
LD       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ld"
NM       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm"
OBJDUMP  		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump"
OBJCOPY  		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy"
SIZE    		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-size"

#function for removing duplicates in a list
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

#source common to all targets
C_SOURCE_FILES += \
$(NRF_SDK_PATH)/components/libraries/button/app_button.c \
./boards/bsp.c \
$(NRF_SDK_PATH)/components/libraries/util/app_error.c \
$(NRF_SDK_PATH)/components/libraries/gpiote/app_gpiote.c \
$(NRF_SDK_PATH)/components/libraries/timer/app_timer.c \
$(NRF_SDK_PATH)/components/libraries/trace/app_trace.c \
$(NRF_SDK_PATH)/components/libraries/uart/retarget.c \
$(NRF_SDK_PATH)/components/libraries/fifo/app_fifo.c \
$(NRF_SDK_PATH)/components/libraries/scheduler/app_scheduler.c \
$(NRF_SDK_PATH)/components/libraries/util/nrf_assert.c \
$(NRF_SDK_PATH)/components/drivers_nrf/hal/nrf_delay.c \
$(NRF_SDK_PATH)/components/drivers_nrf/uart/app_uart_fifo.c \
$(NRF_SDK_PATH)/components/drivers_nrf/pstorage/pstorage.c \
$(NRF_SDK_PATH)/components/softdevice/common/softdevice_handler/softdevice_handler.c \
$(NRF_SDK_PATH)/components/toolchain/system_nrf51.c \
$(NRF_SDK_PATH)/components/ble/ble_services/ble_hrs_c/ble_hrs_c.c \
$(NRF_SDK_PATH)/components/ble/ble_services/ble_bas_c/ble_bas_c.c \
$(NRF_SDK_PATH)/components/ble/common/ble_srv_common.c \
$(NRF_SDK_PATH)/components/ble/device_manager/device_manager_central.c \
$(NRF_SDK_PATH)/components/ble/ble_db_discovery/ble_db_discovery.c \
$(NRF_SDK_PATH)/components/drivers_nrf/spi_master/spi_master.c \
./main.c \
./u8g_arm.c \
./u8g_dev_stdout.c \

# U8G Source files
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_bitmap.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_circle.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_clip.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_com_api_16gr.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_com_api.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_com_io.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_com_null.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_cursor.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_delay.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_dev_ssd1351_128x128.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_ellipse.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_font.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_line.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_ll_api.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_page.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pb14v1.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pb16h1.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pb16h2.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pb16v1.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pb16v2.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pb32h1.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pb8h1.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pb8h1f.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pb8h2.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pb8h8.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pb8v1.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pb8v2.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pb.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pbxh16.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_pbxh24.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_polygon.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_rect.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_rot.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_scale.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_state.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_u16toa.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_u8toa.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_virtual_screen.c
C_SOURCE_FILES += $(U8G_SRC_PATH)/u8g_font_data.c
#C_SOURCE_FILES += $(U8G_FONT_PATH)/u8g_font_data.c

#assembly files common to all targets
ASM_SOURCE_FILES  = $(NRF_SDK_PATH)/components/toolchain/gcc/gcc_startup_nrf51.s

#includes common to all targets
INC_PATHS += -I$(NRF_SDK_PATH)/components/toolchain/gcc
INC_PATHS += -I$(NRF_SDK_PATH)/components/toolchain
INC_PATHS += -I$(NRF_SDK_PATH)/components/libraries/button
INC_PATHS += -I$(NRF_SDK_PATH)/components/ble/ble_services/ble_hrs_c
INC_PATHS += -I$(NRF_SDK_PATH)/components/ble/ble_services/ble_bas_c
INC_PATHS += -I$(NRF_SDK_PATH)/components/ble/common
INC_PATHS += -I$(NRF_SDK_PATH)/components/softdevice/s120/headers
INC_PATHS += -I$(NRF_SDK_PATH)/components/libraries/gpiote
INC_PATHS += -I$(NRF_SDK_PATH)/components/libraries/timer
INC_PATHS += -I$(NRF_SDK_PATH)/components/drivers_nrf/hal
INC_PATHS += -I$(NRF_SDK_PATH)/components/ble/device_manager
INC_PATHS += -I$(NRF_SDK_PATH)/components/ble/device_manager/config
INC_PATHS += -I$(NRF_SDK_PATH)/components/ble/ble_db_discovery
INC_PATHS += -I$(NRF_SDK_PATH)/components/libraries/trace
INC_PATHS += -I$(NRF_SDK_PATH)/components/drivers_nrf/uart
INC_PATHS += -I$(NRF_SDK_PATH)/components/libraries/fifo
INC_PATHS += -I$(NRF_SDK_PATH)/components/softdevice/common/softdevice_handler
INC_PATHS += -I$(NRF_SDK_PATH)/components/libraries/scheduler
INC_PATHS += -I$(NRF_SDK_PATH)/components/drivers_nrf/pstorage
INC_PATHS += -I$(NRF_SDK_PATH)/components/drivers_nrf/pstorage/config
INC_PATHS += -I$(NRF_SDK_PATH)/components/libraries/util
INC_PATHS += -I$(NRF_SDK_PATH)/components/drivers_nrf/spi_master
INC_PATHS += -I./
INC_PATHS += -I./config
INC_PATHS += -I./boards
INC_PATHS += -I$(NRF_SDK_PATH)/examples/bsp
INC_PATHS += -I$(U8G_SRC_PATH)
INC_PATHS += -I$(U8G_FONT_PATH)
INC_PATHS += -I./icons

OBJECT_DIRECTORY = _build
LISTING_DIRECTORY = $(OBJECT_DIRECTORY)
OUTPUT_BINARY_DIRECTORY = $(OBJECT_DIRECTORY)

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) )

#flags common to all targets
CFLAGS  = -D__HEAP_SIZE=128
CFLAGS += -DNRF51
CFLAGS += -DBSP_UART_SUPPORT
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DS120
CFLAGS += -DSOFTDEVICE_PRESENT
#CFLAGS += -DENABLE_DEBUG_LOG_SUPPORT
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DSPI_MASTER_0_ENABLE
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mthumb -mabi=aapcs --std=gnu99
CFLAGS += -Wall -O1
CFLAGS += -ggdb
CFLAGS += -mfloat-abi=soft
# keep every function in separate section. This will allow linker to dump unused functions
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -flto -fno-builtin

CFLAGS += $(CFL)

# keep every function in separate section. This will allow linker to dump unused functions
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -ggdb
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys

# Assembler flags
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -D__HEAP_SIZE=128
ASMFLAGS += -DNRF51
ASMFLAGS += -DBSP_UART_SUPPORT
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DS120
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DBOARD_CUSTOM
ASMFLAGS += -DSPI_MASTER_0_ENABLE
#default target - first one defined
default: clean nrf51422_xxac_s120

#building all targets
all: clean
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e cleanobj
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e nrf51422_xxac_s120 

#target for printing all targets
help:
	@echo following targets are available:
	@echo 	nrf51422_xxac_s120


C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

ASM_SOURCE_FILE_NAMES = $(notdir $(ASM_SOURCE_FILES))
ASM_PATHS = $(call remduplicates, $(dir $(ASM_SOURCE_FILES) ))
ASM_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASM_SOURCE_FILE_NAMES:.s=.o) )

vpath %.c $(C_PATHS)
vpath %.s $(ASM_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

nrf51422_xxac_s120: OUTPUT_FILENAME := nrf51422_xxac_s120
nrf51422_xxac_s120: LINKER_SCRIPT=$(NRF_SDK_PATH)/components/toolchain/gcc/gcc_nrf51_s120_xxac.ld
nrf51422_xxac_s120: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e finalize

## Create build directories
$(BUILD_DIRECTORIES):
	echo $(MAKEFILE_NAME)
	$(MK) $@

# Create objects from C SRC files
$(OBJECT_DIRECTORY)/%.o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<

# Assemble files
$(OBJECT_DIRECTORY)/%.o: %.s
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(CC) $(ASMFLAGS) $(INC_PATHS) -c -o $@ $<


# Link
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out


## Create binary .bin file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

finalize: genbin genhex echosize

genbin:
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
genhex: 
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

echosize:
	-@echo ""
	$(NO_ECHO)$(SIZE) $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	-@echo ""

clean:
	$(RM) $(BUILD_DIRECTORIES)

cleanobj:
	$(RM) $(BUILD_DIRECTORIES)/*.o

flash: $(MAKECMDGOALS)
	@echo Flashing: $(OUTPUT_BINARY_DIRECTORY)/$<.hex
	nrfjprog --reset --program $(OUTPUT_BINARY_DIRECTORY)/$<.hex
