PYTHON = python2

CFLAGS = -std=c11 -O2 -Wall -Wextra

exes = exes/addxor_tool exes/cfwtool exes/pack_tool exes/xor
DATA_FILES := $(wildcard data/*.*) cfwmode/nat_patch.bin cfwmode/agb_patch.bin cfwmode/twl_patch.bin

.PHONY: all
all: cfw.dat

.PHONY: distclean
distclean: clean
	@rm -rf release/cfw.dat release/ninjhax release/mset

.PHONY: clean
clean:
	@make -C cfwtools clean
	@make -C cfwmode clean
	@make -C msethax clean
	@rm -f $(exes) payload.bin data.bin cfw.dat

cfw.dat: payload.bin cfwmode/*.bin data.bin msethax/mset.bin
	@cp spiderhax/Launcher.dat $@
	@cp cfwtools/cfwtools.bin payload2.bin
	@$(PYTHON) exes/insert.py $@ payload2.bin 0x22000
	@$(PYTHON) exes/insert.py $@ payload.bin 0x50000
	@$(PYTHON) exes/insert.py $@ data.bin 0x140000
	@$(PYTHON) exes/insert.py $@ msethax/mset.bin 0
	@dd if=/dev/zero bs=$$((2097152-$$(stat -c %s $@))) count=1 >> $@

.PHONY: msethax/mset.bin
msethax/mset.bin:
	make -C $(dir $@) all

data.bin: 
	@exes/pack_tool $(DATA_FILES) $@
	@exes/xor $@ exes/xorpad/data.xor
	@rm $@
	@mv $@.out $@

.PHONY: cfwmode/*.bin
cfwmode/*.bin:
	@cd cfwmode && make

payload.bin: cfwtools/cfwtools.bin exes/addxor_tool
	@exes/addxor_tool $< $@ 0x67893421 0x12756342

.PHONY: cfwtools/cfwtools.bin
cfwtools/cfwtools.bin: exes/addxor_tool
	@make -C $(dir $@) all
	@dd if=/dev/zero bs=$$((917504-$$(stat -c %s $@))) count=1 >> $@