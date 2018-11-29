################################################################################
# These are variables for the GBA toolchain build
################################################################################

PROGNAME = Animation

OFILES = gba.o font.o main.o tl06.o snoopframes.o didnotdraw.o

################################################################################
# These are various settings used to make the GBA toolchain work
# DO NOT EDIT BELOW.
################################################################################

.PHONY: all
all : CFLAGS += $(CRELEASE) -I../shared
all : LDFLAGS += $(LDRELEASE)
all: $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

include /opt/cs2110-tools/GBAVariables.mak

# Son, it's time to be an adult and use adult prototypes
CFLAGS += -Wstrict-prototypes -Wold-style-definition

debug : CFLAGS += $(CDEBUG) -I../shared
debug : LDFLAGS += $(LDDEBUG)
debug : $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

$(PROGNAME).gba : $(PROGNAME).elf
	@echo "[LINK] Linking objects together to create $(PROGNAME).gba"
	@$(OBJCOPY) -O binary $(PROGNAME).elf $(PROGNAME).gba

$(PROGNAME).elf : crt0.o $(GCCLIB)/crtbegin.o $(GCCLIB)/crtend.o $(GCCLIB)/crti.o $(GCCLIB)/crtn.o $(OFILES) libc_sbrk.o
	$(CC) -o $(PROGNAME).elf $^ $(LDFLAGS)

.PHONY : emu
emu : med

.PHONY : vba
vba : CFLAGS += $(CRELEASE) -I../shared
vba : LDFLAGS += $(LDRELEASE)
vba : $(PROGNAME).gba
	@echo "[EXECUTE] Running Emulator VBA-M"
	@echo "          Please see emulator.log if this fails"
	@vbam $(VBAOPT) $(PROGNAME).gba >emulator.log 2>&1

.PHONY : med
med : CFLAGS += $(CRELEASE) -I../shared
med : LDFLAGS += $(LDRELEASE)
med : $(PROGNAME).gba
	@echo "[EXECUTE] Running emulator Mednafen"
	@echo "          Please see emulator.log if this fails"
# Default mednafen keybinds are "Gaaarbage," in the words of Farzam
	@mkdir -p ~/.mednafen/
	@cp mednafen-09x.cfg ~/.mednafen/
	@mednafen $(MEDOPT) $(PROGNAME).gba >emulator.log 2>&1

.PHONY : submit
submit: clean
	@rm -f submission.tar.gz
	@tar czvf submission.tar.gz *

.PHONY : clean
clean :
	@echo "[CLEAN] Removing all compiled files"
	rm -f *.o *.elf *.gba *.log */*.o
