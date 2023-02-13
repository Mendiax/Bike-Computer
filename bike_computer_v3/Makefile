################################################################################
#                                      setup                                   #
################################################################################

PICODIR := ./pico #/mnt/h
BUILD := build
BUILD_LOG := $(BUILD)/build.log
PICO_MAIN_FILE := pico_main
PICO_TEST_FILE := pico_test
PICO_GSM_CONSOLE_FILE := pico_gsm_console

SUPRESS_WARINGS := supress_warnings.txt


NPROCS:=$(shell grep -c ^processor /proc/cpuinfo)

$(info [INFO] using $(NPROCS) threads)

### WSL ###
WSL = $(grep wsl -i /proc/version)
ifeq ($(WSL),)
	PICO_DRIVE_LETTER := H
	PICO_MAIN_FILE_PATH:=$(shell wslpath -w $(BUILD)/$(PICO_MAIN_FILE).uf2)
	PICO_TEST_FILE_PATH:=$(shell wslpath -w $(BUILD)/$(PICO_TEST_FILE).uf2)
$(info [INFO] Running WSL)
$(info [INFO] [WSL] pico letter $(PICO_DRIVE_LETTER))
endif
### END ###

# can be overriden with "make BUILD_WITH=Unix\ Makefiles"
ifdef BUILD_WITH
  	CMAKE_BUILDER = Unix\ Makefiles
else
  	CMAKE_COMMAND = $(shell command -v ninja 2> /dev/null)
	ifneq ($(CMAKE_COMMAND),)
      	CMAKE_BUILDER = "Ninja"
  	else
    	CMAKE_BUILDER = "Unix Makefiles"
  	endif
endif

$(info [INFO] [CMAKE] Building with $(CMAKE_BUILDER))

define build_info
$(info [INFO] BUILD FINISHED)
$(info [INFO] build output saved to $(BUILD_LOG))
endef

################################################################################
#                                  run locally                                 #
################################################################################


all:
	# $(MAKE) -C $(BUILD) -j$(NPROCS) && echo "BUILD FINISHED"
	cmake --build ./build --parallel $(NPROCS) 2>&1 | tee $(BUILD_LOG)
	$(call build_info)

main:
	cmake --build ./build --parallel $(NPROCS) --target $(PICO_MAIN_FILE) && echo "BUILD FINISHED"

test:
	cmake --build ./build --parallel $(NPROCS) --target $(PICO_TEST_FILE) && echo "BUILD FINISHED"

gsm:
	cmake --build ./build --parallel $(NPROCS) --target $(PICO_GSM_CONSOLE_FILE) && echo "BUILD FINISHED"

sim:
	$(MAKE) -C $(BUILD) -j$(NPROCS)

parser:
	bison -o source/lib/Parser/src/parser_bison.cpp -d source/lib/Parser/parser.y
	flex -Cr -o source/lib/Parser/src/lexer.cpp source/lib/Parser/lexer.l


CMAKE_ARGS := -G$(CMAKE_BUILDER) -S ./ -B $(BUILD) -DPICO_MAIN_FILE=$(PICO_MAIN_FILE) -DPICO_TEST_FILE=$(PICO_TEST_FILE) -DCMAKE_EXPORT_COMPILE_COMMANDS=1

#-DCMAKE_VERBOSE_MAKEFILE=ON
cmake: folder parser
	cmake $(CMAKE_ARGS) -DCMAKE_BUILD_TYPE=Debug

cmaker: folder parser
	cmake $(CMAKE_ARGS) -DCMAKE_BUILD_TYPE=Release



warnings:
	$(info [INFO] checking warings in $(BUILD_LOG))
	./tools/warnings.sh $(BUILD_LOG)




.PHONY: release
release:
	cp $(BUILD)/$(PICO_MAIN_FILE).uf2 ./release



################################################################################
#                                  run locally                                 #
################################################################################

cw_all:
	./boot/boot.sh -o b && $(MAKE) main && $(MAKE) write_main && sleep 2 && ./serial.bat

cwt_all:
	./boot/boot.sh -o b && $(MAKE) test && $(MAKE) write_test && sleep 2 && ./serial.bat

.PHONY: boot
boot:
	./boot/boot.sh -o b

.PHONY: restart
restart:
	./boot/boot.sh -o r

w: write_main
wt: write_test
write_main:
	./boot/upload.sh -p '$(PICO_MAIN_FILE_PATH)' -d $(PICO_DRIVE_LETTER)

write_test: boot
	./boot/upload.sh -p '$(PICO_TEST_FILE_PATH)' -d $(PICO_DRIVE_LETTER)

write_gsm_console: boot
	#mkdir -p $(PICODIR)
	mount -t drvfs $(PICO_DRIVE_LETTER): $(PICODIR)
	cp $(BUILD)/$(PICO_GSM_CONSOLE_FILE).uf2 $(PICODIR)

# compile & write
cw: main write_main
	echo "cw main"

cwt: test write_test
	echo "cw test"

cwgsm: gsm write_gsm_console
	echo "cw gsm console"

################################################################################
#                               clean                                          #
################################################################################

folder:
	mkdir -p $(BUILD)

clean:
	# $(MAKE) clean -C $(BUILD)
	rm -rf  $(BUILD)
	mkdir -p $(BUILD)
	# find . -name "*.s" -type f -delete
	# find . -name "*.gch" -type f -delete

reset: clean cmake all
	echo "clean make done!"

# test:
# 	$(MAKE) test -C $(BUILD)