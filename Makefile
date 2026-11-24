# =============================================================================
#  Makefile: STM32F103 Blue Pill (GNU make + arm-none-eabi-gcc, Windows)
#
#  Project layout:
#    app/                  application code (main)
#    libs/drivers/         submodule: util/, mcu/, bsp/
#
#  Commands:
#    make                  build final.elf and final.bin
#    make load             flash final.bin with st-flash
#    make probe            check that the ST-Link can see the board
#    make openocd          start OpenOCD (leave it running)
#    make gdb              start GDB and connect to OpenOCD (second terminal)
#    make clean            delete build outputs only
# =============================================================================


# -----------------------------------------------------------------------------
# 0. Shell selection
#    On Windows, use cmd.exe so the mkdir/rmdir commands below work.
#    Git Bash in PATH would otherwise use sh
# -----------------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
SHELL := cmd.exe
endif


# -----------------------------------------------------------------------------
# 1. Toolchain
# -----------------------------------------------------------------------------
PREFIX   := arm-none-eabi-
CC       := $(PREFIX)gcc
OBJCOPY  := $(PREFIX)objcopy
OBJDUMP  := $(PREFIX)objdump
NM       := $(PREFIX)nm
SIZE     := $(PREFIX)size
GDB      := $(PREFIX)gdb

# ST-Link command-line tools
FLASH    := st-flash --reset write
PROBE    := st-info --probe

# xpack OpenOCD, Forward slashes avoid escaping problems in make.
OPENOCD_DIR  := C:/Users/Saai/DATA/Applications/ARM/xpack-openocd-0.12.0-3-win32-x64/xpack-openocd-0.12.0-3
OPENOCD      := $(OPENOCD_DIR)/bin/openocd.exe
OPENOCD_INTF := $(OPENOCD_DIR)/openocd/scripts/interface/stlink-v2.cfg
OPENOCD_TGT  := $(OPENOCD_DIR)/openocd/scripts/target/stm32f1x.cfg


# -----------------------------------------------------------------------------
# 2. MCU settings
# -----------------------------------------------------------------------------
MCU        := cortex-m3
# chip variant
DEFS       := -DSTM32F103xB
# Flash Start Address
FLASH_ADDR := 0x08000000


# -----------------------------------------------------------------------------
# 3. Project and library folders
#    Change these when the layout changes. Everything else is derived from them.
# -----------------------------------------------------------------------------
# output name: final.elf, final.bin
TARGET   := final
# application source folder
APP_DIR  := app

# ---- Library folders (submodules) ---------------------------------------
BOARD    := stm32f103
# flat, no (include, src) split
BSP      := bsp/$(BOARD)

# include/ holds .h files (passed to the compiler with -I)
# src/ holds .c and .s files (compiled)
MCU_INC  := mcu/include
MCU_SRC  := mcu/src
UTIL_INC := util/include
UTIL_SRC := util/src

# Folders with .c and .s files. wildcard does not search subfolders,
# so list each src/ folder directly.
SRC_DIRS := $(APP_DIR) $(MCU_SRC) $(UTIL_SRC) $(DEV_SRC) $(BSP)

# Folders with .h files. BSP appears in both lists because it is flat
INC_DIRS := $(APP_DIR) $(MCU_INC) $(UTIL_INC) $(DEV_INC) $(BSP)

# Build outputs
OBJ_DIR  := Objects
LOG_DIR  := logs
ELF      := $(OBJ_DIR)/$(TARGET).elf
BIN      := $(OBJ_DIR)/$(TARGET).bin
MAP      := $(LOG_DIR)/$(TARGET).map

# Linker script
LINKER   := $(BSP)/bp_linker.ld

#$(info BSP=$(BSP))
#$(info LINKER=$(LINKER))
#$(info ELF=$(ELF))


# -----------------------------------------------------------------------------
# 4. Source discovery
#    $(wildcard folder/*.c): Finds the .c files in each folder.
#    Subfolders are not searched, so keep each folder's sources flat.
# -----------------------------------------------------------------------------
C_SRCS   := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.c))
ASM_SRCS := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.s))

# Object files go flat into $(OBJ_DIR). notdir strips the folder part
# Two source files with the same name in different folders will collide.
OBJS     := $(addprefix $(OBJ_DIR)/,$(notdir $(C_SRCS:.c=.o)) \
                                    $(notdir $(ASM_SRCS:.s=.o)))

# vpath tells make where to look for .c and .s files
# pattern rules below work regardless of which folder a source file is in.
vpath %.c $(SRC_DIRS)
vpath %.s $(SRC_DIRS)


# -----------------------------------------------------------------------------
# 5. Include paths
#    Each folder in SRC_DIRS becomes a -I entry
#    resolves from any folder without a path
# -----------------------------------------------------------------------------
INCLUDES := $(addprefix -I,$(INC_DIRS))


# -----------------------------------------------------------------------------
# 6. Compiler and linker flags
# -----------------------------------------------------------------------------
# -c           compile only, do not link
# -Og/-O0      optimisation level (you use -O0, which is easiest to debug)
# -g3          full debug info for GDB
# -Wall        enable common warnings
# -MMD -MP     write .d dependency files so headers trigger rebuilds
CFLAGS   := -c -mcpu=$(MCU) -mthumb -mfloat-abi=soft $(DEFS) $(INCLUDES) \
            -std=gnu11 -Wall -Wno-unused -O0 -g3 -MMD -MP

ASFLAGS  := -c -mcpu=$(MCU) -mthumb -mfloat-abi=soft -g3

# --specs=nano.specs  use the small newlib (printf, malloc)
# -T                  use this linker script
# -Wl,-Map=...        write a memory map to logs/
# Libraries (LIBS) must come after the objects that use them.
LDFLAGS  := -mcpu=$(MCU) -mthumb -mfloat-abi=soft --specs=nano.specs \
            -T$(LINKER) -Wl,-Map=$(MAP)
LIBS     := -lm


# -----------------------------------------------------------------------------
# 7. Folder helpers
#    Call them with: $(call MKDIR,folder)  or  $(call RMDIR,folder)
# -----------------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
  MKDIR = if not exist $(1) mkdir $(1)
  RMDIR = if exist $(1) rmdir /s /q $(1)
else
  MKDIR = mkdir -p $(1)
  RMDIR = rm -rf $(1)
endif


# -----------------------------------------------------------------------------
# 8. Build rules
# -----------------------------------------------------------------------------
.PHONY: all load probe openocd gdb clean examine-sections examine-symbols dump

# Default target: "make" with no arguments builds the firmware.
all: $(ELF) $(BIN)
	$(SIZE) $(ELF)

# Link all object files into the ELF
$(ELF): $(OBJS) $(LINKER) | $(OBJ_DIR) $(LOG_DIR)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

# Convert the ELF to a raw binary. st-flash writes this file.
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

# Compile each C source. $< is the source (found through vpath) and $@ the object.
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEBUG_FLAG) $< -o $@

# Assemble each .s source (if the BSP has any).
$(OBJ_DIR)/%.o: %.s | $(OBJ_DIR)
	$(CC) $(ASFLAGS) $< -o $@

# Create output folders on demand.
# The | before them in the rules above means "make sure this exists first, but don't rebuild everything if it changes".
$(OBJ_DIR) $(LOG_DIR):
	$(call MKDIR,$@)

# Pull in the dependency files written by -MMD. Changing a header now
# rebuilds the .c files that include it.
-include $(OBJS:.o=.d)


# -----------------------------------------------------------------------------
# 9. Flash and debug
# -----------------------------------------------------------------------------
# Program the binary at the flash start address and reset the board
load: $(BIN)
	$(FLASH) $(BIN) $(FLASH_ADDR)

# Check that the ST-Link and target are reachable
probe:
	$(PROBE)

# OpenOCD runs as a server that GDB connects to
openocd:
	$(OPENOCD) -f $(OPENOCD_INTF) -f $(OPENOCD_TGT)

# Start GDB, connect to OpenOCD on port 3333, reset the MCU, and load the ELF
gdb: $(ELF)
	$(GDB) $(ELF) \
	  -ex "target extended-remote localhost:3333" \
	  -ex "monitor reset halt" \
	  -ex "load"


# -----------------------------------------------------------------------------
# 10. Inspection
# -----------------------------------------------------------------------------
examine-sections: $(ELF)
	$(OBJDUMP) -h $(ELF)

examine-symbols: $(ELF)
	$(NM) $(ELF)

dump: $(ELF) | $(LOG_DIR)
	$(OBJDUMP) -d $(ELF) > $(LOG_DIR)/assembly_dump.txt
	$(OBJDUMP) -h $(ELF) > $(LOG_DIR)/section_contents.txt
	$(OBJDUMP) -s $(ELF) > $(LOG_DIR)/section_contents_detailed.txt
	$(NM) $(ELF)         > $(LOG_DIR)/symbols.txt


# -----------------------------------------------------------------------------
# 11. Clean
#     Deletes only the Objects and logs folders.
# -----------------------------------------------------------------------------
clean:
	$(call RMDIR,$(OBJ_DIR))
	$(call RMDIR,$(LOG_DIR))