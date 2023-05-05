################################################################################
#                                      setup                                   #
################################################################################

PICODIR := ./pico #/mnt/h
BUILD := build
BUILD_LOG := $(BUILD)/build.log
PICO_MAIN_FILE := pico_main
PICO_TEST_FILE := target_test
PICO_GSM_CONSOLE_FILE := pico_gsm_console

SUPRESS_WARINGS := supress_warnings.txt

PROJ_DIR=$(shell pwd)

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

ifneq ($(V),1)
    CMAKE_VERBOSE_MAKEFILE =
else
    CMAKE_VERBOSE_MAKEFILE = 1
$(info [INFO] [CMAKE] Veroose flag on $(CMAKE_VERBOSE_MAKEFILE))
endif

ifeq ($(H),1)
    BUILD_TARGET = host
else
    BUILD_TARGET = rp2040
endif

ifeq ($(D),1)
    BUILD_TYPE = Debug
else
    BUILD_TYPE = Release
endif

ifeq ($(L),1)
    PICO_TEST_HOST = local
else
    PICO_TEST_HOST = ssh
endif

ifeq ($(VALGRIND),1)
	PICO_TEST_VALGRIND = valgrind
	BUILD_TYPE = Debug
	PICO_TEST_HOST = local
	BUILD_TARGET = host
else
    VALGRIND = normal
endif


$(info [INFO] [CMAKE] build target = $(BUILD_TARGET))
$(info [INFO] [CMAKE] build type = $(BUILD_TYPE))
$(info [INFO] [CMAKE] hw test host = $(PICO_TEST_HOST))
$(info [INFO] [CMAKE] valgrind = $(PICO_TEST_VALGRIND))



$(info [INFO] [CMAKE] Building with $(CMAKE_BUILDER))

define build_info
$(info [INFO] BUILD FINISHED)
$(info [INFO] build output saved to $(BUILD_LOG))
endef



################################################################################
#                                  run locally                                 #
################################################################################


all: clean_tests_header
	cmake --build ./build --parallel $(NPROCS) 2>&1 | tee $(BUILD_LOG)
	$(call build_info)

full_build: cmake
	cmake --build ./build --parallel $(NPROCS) 2>&1 | tee $(BUILD_LOG)
	$(call build_info)

main:
	cmake --build ./build --parallel $(NPROCS) --target $(PICO_MAIN_FILE) && echo "BUILD FINISHED"

help:
	@echo "\nFlags for \'make cmake\':"
	@echo "D=1 -> cmake debug build"
	@echo "H=1 -> cmake host tests build. Run tests that do not need hw"
	@echo "V=1 -> cmake verobse build"
	@echo "L=1 -> cmake run test locally (pico is connected to this system) L=0 -> tests will be run over ssh"
	@# @echo "=================================================================================================="
	@echo "\nBuild process:"
	@echo "make cmake <args>              // build cmake"
	@echo "make all                       // build program"
	@echo "progam build in $(BUILD)/$(PICO_MAIN_FILE).uf2"
	@echo "make write_main                // send program to device"

	@echo "\nBuild test process:"
	@echo "make cmake <args>              // build cmake"
	@echo "make all                       // build program"
	@echo "make ctest                     // run program"






# test:
# 	cmake --build ./build --parallel $(NPROCS) --target $(PICO_TEST_FILE) && echo "BUILD FINISHED"

# gsm:
# 	cmake --build ./build --parallel $(NPROCS) --target $(PICO_GSM_CONSOLE_FILE) && echo "BUILD FINISHED"


ctest:
	cd $(BUILD) && ctest --output-on-failure -V

ctest_all: all
	$(MAKE) ctest

parser:
	bison -o source/lib/Parser/src/parser_bison.cpp -d source/lib/Parser/parser.y
	flex -Cr -o source/lib/Parser/src/lexer.cpp source/lib/Parser/lexer.l


CMAKE_ARGS := -G$(CMAKE_BUILDER) -S ./ -B $(BUILD) -DPICO_MAIN_FILE=$(PICO_MAIN_FILE) -DPICO_TEST_FILE=$(PICO_TEST_FILE) -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_VERBOSE_MAKEFILE=$(CMAKE_VERBOSE_MAKEFILE) \
			  -DPICO_PLATFORM=$(BUILD_TARGET) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DPICO_TEST_HOST=$(PICO_TEST_HOST) -DPICO_TEST_VALGRIND=$(PICO_TEST_VALGRIND)

#-DCMAKE_VERBOSE_MAKEFILE=ON
cmake: clean folder parser
	cmake $(CMAKE_ARGS)

cmaker: clean folder parser
	cmake $(CMAKE_ARGS) -DCMAKE_BUILD_TYPE=Release

cmake_host: clean folder parser
	cmake $(CMAKE_ARGS) -DCMAKE_BUILD_TYPE=Debug -DPICO_PLATFORM=host
# -DPICO_SDK_PRE_LIST_DIRS=$(PROJ_DIR)/source/external/pico-host-sdl


warnings:
	$(info [INFO] checking warings in $(BUILD_LOG))
	./tools/warnings.sh $(BUILD_LOG)




# .PHONY: release
# release:
# 	cp $(BUILD)/$(PICO_MAIN_FILE).uf2 ./release



################################################################################
#                                  run locally                                 #
################################################################################

serial: restart
	sleep 4 && ./tools/serial.bat

cw_all:
	./tools/boot/boot.sh -o b && $(MAKE) all && $(MAKE) write_main && sleep 2 && ./tools/serial.bat

cwt_all:
	cd tools && ./boot/boot.sh -o b && $(MAKE) test && $(MAKE) write_test && sleep 2 && ./serial.bat

.PHONY: boot
boot:
	./tools/boot/boot.sh -o b

.PHONY: restart
restart:
	./tools/boot/boot.sh -o r

w: write_main
wt: write_test
write_main:
	./tools/boot/upload.sh -p '$(PICO_MAIN_FILE_PATH)' -d $(PICO_DRIVE_LETTER)

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

plot:
	python3 tools/plotter.py

################################################################################
#                               clean                                          #
################################################################################

folder:
	mkdir -p $(BUILD)

clean_tests_header:
	rm -f tests/hw/tests/test_cases.h

clean: clean_tests_header
	rm -rf  $(BUILD)
	mkdir -p $(BUILD)

reset: clean cmake all
	@echo "clean make done!"

# test:
# 	$(MAKE) test -C $(BUILD)