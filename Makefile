################################################################################
#                                      setup                                   #
################################################################################

ifeq ($(HOST),1)
    H = 1
endif

ifeq ($(H),1)
    BUILD_TARGET = host
else
    BUILD_TARGET = rp2040
endif

ifeq ($(DEBUG),1)
	D = 1
endif

ifeq ($(D),1)
	BUILD_TYPE = Debug
else
	BUILD_TYPE = Release
endif

PICODIR := ./pico #/mnt/h
BUILD := build/$(BUILD_TARGET)/$(BUILD_TYPE)
BUILD_LOG := $(BUILD)/build.log
PICO_MAIN_FILE := pico_main
PICO_TEST_FILE := target_test
PICO_GSM_CONSOLE_FILE := pico_gsm_console

PICO_MAIN_FILE_PATH_RELATIVE:=$(BUILD)/$(PICO_MAIN_FILE).uf2
PICO_TEST_FILE_PATH_RELATIVE:=$(BUILD)/$(PICO_TEST_FILE).uf2

SUPRESS_WARINGS := supress_warnings.txt

PROJ_DIR=$(shell pwd)

NPROCS:=$(shell grep -c ^processor /proc/cpuinfo)

$(info [INFO] using $(NPROCS) threads)

### WSL ###
WSL = $(grep wsl -i /proc/version)
ifeq ($(WSL),)
	PICO_DRIVE_LETTER := E
	PICO_MAIN_FILE_PATH:=$(shell wslpath -w $(PICO_MAIN_FILE_PATH_RELATIVE))
	PICO_TEST_FILE_PATH:=$(shell wslpath -w $(BUILD)/$(PICO_TEST_FILE))
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

ifeq ($(VERBOSE),1)
	V = 1
endif

ifneq ($(V),1)
	CMAKE_VERBOSE_MAKEFILE =
else
	CMAKE_VERBOSE_MAKEFILE = 1
	$(info [INFO] [CMAKE] Verbose flag on $(CMAKE_VERBOSE_MAKEFILE))
endif

ifeq ($(LOCAL),1)
	L = 1
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


rebuild: clean parser cmake
	cmake --build $(BUILD) --parallel $(NPROCS) 2>&1 | tee $(BUILD_LOG)
	$(call build_info)

build:
	cmake --build $(BUILD) --parallel $(NPROCS) --target $(PICO_MAIN_FILE) && echo "BUILD FINISHED"

help:
	@echo "\nFlags:"
	@echo "D=1 or DEBUG=1                 // Debug build (default: Release)"
	@echo "H=1 or HOST=1                  // Host/test build (default: rp2040 hardware)"
	@echo "V=1 or VERBOSE=1               // Verbose cmake output"
	@echo "L=1 or LOCAL=1                 // Run tests locally (default: L=0 for ssh)"
	@echo "VALGRIND=1                     // Run with valgrind (implies D=1 H=1 L=1)"

	@echo "\nCommon Commands:"
	@echo "make rebuild                   // Build program from scratch"
	@echo "make build                     // Build main target"
	@echo "make cmake                     // Configure cmake"
	@echo "make ctest                     // Run tests"
	@echo "make ctest_rebuild             // Rebuild and run tests"
	@echo "make test_all                  // Full test suite (host + hw)"
	@echo "make write_main                // Upload program to device"
	@echo "make serial                    // View serial output"
	@echo "make cw_all                    // Build and upload with serial monitor"
	@echo "make clean                     // Clean build directory"
	@echo "make plot                      // Plot data from logs"


ctest: build
	cd $(BUILD) && ctest --output-on-failure -V -j $(NPROCS)

ctest_rebuild: rebuild
	$(MAKE) ctest


test_all:
	$(MAKE) ctest_rebuild L=$(L) D=0 H=1
	$(MAKE) warnings
	$(MAKE) ctest_rebuild L=$(L) D=0
	$(MAKE) warnings



parser:
	bison -o source/lib/Parser/src/parser_bison.cpp -d source/lib/Parser/parser.y
	flex -Cr -o source/lib/Parser/src/lexer.cpp source/lib/Parser/lexer.l
# 	bison -o source/lib/Parser/src/parser_gpx_bison.cpp -d source/lib/Parser/parser_gpx.y
# 	flex -Cr -o source/lib/Parser/src/lexer_gpx.cpp source/lib/Parser/lexer_gpx.l


CMAKE_ARGS := -G$(CMAKE_BUILDER) -S ./ \
	-B $(BUILD) \
	-DPICO_MAIN_FILE=$(PICO_MAIN_FILE) \
	-DPICO_TEST_FILE=$(PICO_TEST_FILE) \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
	-DCMAKE_VERBOSE_MAKEFILE=$(CMAKE_VERBOSE_MAKEFILE) \
	-DPICO_PLATFORM=$(BUILD_TARGET) \
	-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	-DPICO_TEST_HOST=$(PICO_TEST_HOST) \
	-DPICO_TEST_VALGRIND=$(PICO_TEST_VALGRIND) \
	# -DPICO_SDK_PRE_LIST_DIRS=$(PROJ_DIR)/source/external/pico-host-sdl


#-DCMAKE_VERBOSE_MAKEFILE=ON
cmake: clean folder parser
	cmake $(CMAKE_ARGS)

# -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DPICO_PLATFORM=$(BUILD_TARGET)
# cmaker: clean folder parser
# 	cmake $(CMAKE_ARGS) -DCMAKE_BUILD_TYPE=Release

# cmake_host: clean folder parser
# 	cmake $(CMAKE_ARGS) -DCMAKE_BUILD_TYPE=Debug -DPICO_PLATFORM=host

# -DPICO_SDK_PRE_LIST_DIRS=$(PROJ_DIR)/source/external/pico-host-sdl


warnings:
	$(info [INFO] checking warings in $(BUILD_LOG))
	./tools/warnings.sh $(BUILD_LOG)


.PHONY: release
release:
	cp $(BUILD)/$(PICO_MAIN_FILE).uf2 ./release



################################################################################
#                                  run locally                                 #
################################################################################

serial: restart
	sleep 4 && ./tools/serial.bat

cw_all:
	./tools/boot/boot.sh -o b && $(MAKE) all && $(MAKE) write_main && sleep 2 && ./tools/serial.bat

cw_tool:
	$(MAKE) all && $(MAKE) write_picotool

write_picotool:
	picotool load -x ./$(PICO_MAIN_FILE_PATH_RELATIVE) -f || picotool load -x ./$(PICO_MAIN_FILE_PATH_RELATIVE) -F

write_openocd:
	openocd -s /home/bartek/pico/pico-sdk/../openocd/tcl -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program $(BUILD)/pico_main.elf verify reset exit"


cwt_all:
	cd tools && ./boot/boot.sh -o b && $(MAKE) test && $(MAKE) write_test && sleep 2 && ./serial.bat

.PHONY: boot
boot:
	./tools/boot/boot.sh -o b -p $(PORT)

.PHONY: restart
restart:
	./tools/boot/boot.sh -o r -p $(PORT)

w: write_main
wt: write_test
write_main:
	./tools/boot/upload.sh -p '$(PICO_MAIN_FILE_PATH)' -d $(PICO_DRIVE_LETTER)
	# picotool load -v -x $(PICO_MAIN_FILE_PATH) -f

write_test: boot
	./boot/upload.sh -p '$(PICO_TEST_FILE_PATH)' -d $(PICO_DRIVE_LETTER)


# compile & write
cw: main write_main
	echo "cw main"

cwt: test write_test
	echo "cw test"

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
