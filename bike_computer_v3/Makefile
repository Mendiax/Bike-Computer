PICODIR := ./pico #/mnt/h
BUILD := build
PICO_MAIN_FILE := pico_main
PICO_TEST_FILE := pico_test
PICO_GSM_CONSOLE_FILE := pico_gsm_console

PICO_DRIVE_LETTER := I

NPROCS:=$(shell grep -c ^processor /proc/cpuinfo)
PICO_MAIN_FILE_PATH:=$(shell wslpath -w $(BUILD)/$(PICO_MAIN_FILE).uf2)
PICO_TEST_FILE_PATH:=$(shell wslpath -w $(BUILD)/$(PICO_TEST_FILE).uf2)

CMAKE_BUILDER := "Ninja"
# CMAKE_BUILDER := "Unix Makefiles"

all:
	# $(MAKE) -C $(BUILD) -j$(NPROCS) && echo "BUILD FINISHED"
	cmake --build ./build --parallel $(NPROCS)

main:
	cmake --build ./build --parallel $(NPROCS) --target $(PICO_MAIN_FILE) && echo "BUILD FINISHED"

test:
	cmake --build ./build --parallel $(NPROCS) --target $(PICO_TEST_FILE) && echo "BUILD FINISHED"

gsm:
	cmake --build ./build --parallel $(NPROCS) --target $(PICO_GSM_CONSOLE_FILE) && echo "BUILD FINISHED"

sim:
	$(MAKE) -C $(BUILD) -j$(NPROCS)

parser:
	bison -o lib/Parser/src/parser_bison.cpp -d lib/Parser/parser.y
	flex -Cr -o lib/Parser/src/lexer.cpp lib/Parser/lexer.l

#-DCMAKE_VERBOSE_MAKEFILE=ON
cmake: parser
	cmake -G$(CMAKE_BUILDER) -S ./ -B $(BUILD) -DPICO_MAIN_FILE=$(PICO_MAIN_FILE) -DPICO_TEST_FILE=$(PICO_TEST_FILE) -DCMAKE_BUILD_TYPE=Debug

cmaker: parser
	cmake -G$(CMAKE_BUILDER) -S ./ -B $(BUILD) -DPICO_MAIN_FILE=$(PICO_MAIN_FILE) -DPICO_TEST_FILE=$(PICO_TEST_FILE) -DCMAKE_BUILD_TYPE=Release

.PHONY: release
release:
	cp $(BUILD)/$(PICO_MAIN_FILE).uf2 ./release


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
	#mkdir -p $(PICODIR)
	# mount -t drvfs $(PICO_DRIVE_LETTER): $(PICODIR)
	# cp $(BUILD)/$(PICO_MAIN_FILE).uf2 $(PICODIR)

	./boot/upload.sh -p '$(PICO_MAIN_FILE_PATH)' -d $(PICO_DRIVE_LETTER)

write_test: boot
	#mkdir -p $(PICODIR)
	# mount -t drvfs $(PICO_DRIVE_LETTER): $(PICODIR)
	# cp $(BUILD)/$(PICO_TEST_FILE).uf2 $(PICODIR)
	./boot/upload.sh -p '$(PICO_TEST_FILE_PATH)'

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

clean:
	# $(MAKE) clean -C $(BUILD)
	rm -rf  $(BUILD)
	mkdir $(BUILD)
	# find . -name "*.s" -type f -delete
	# find . -name "*.gch" -type f -delete

reset: clean cmake all
	echo "clean make done!"



getFatFSlib:
	git clone https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico.git

# test:
# 	$(MAKE) test -C $(BUILD)