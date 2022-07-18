PICODIR := ./pico #/mnt/h
BUILD := build
PICO_MAIN_FILE := pico_main
PICO_TEST_FILE := pico_test
PICO_GSM_CONSOLE_FILE := pico_gsm_console

PICO_DRIVE_LETTER := D

all:
	$(MAKE) -C $(BUILD) -j4

cmake:
	cmake -S ./ -B $(BUILD) -DPICO_MAIN_FILE=$(PICO_MAIN_FILE) -DPICO_TEST_FILE=$(PICO_TEST_FILE) -DCMAKE_BUILD_TYPE=Debug

cmaker:
	cmake -S ./ -B $(BUILD) -DPICO_MAIN_FILE=$(PICO_MAIN_FILE) -DPICO_TEST_FILE=$(PICO_TEST_FILE) -DCMAKE_BUILD_TYPE=Release

w: write_main
wt: write_test
write_main:
	mount -t drvfs $(PICO_DRIVE_LETTER): $(PICODIR)
	cp $(BUILD)/$(PICO_MAIN_FILE).uf2 $(PICODIR)

write_test:
	mount -t drvfs $(PICO_DRIVE_LETTER): $(PICODIR)
	cp $(BUILD)/$(PICO_TEST_FILE).uf2 $(PICODIR)

write_gsm_console:
	mount -t drvfs $(PICO_DRIVE_LETTER): $(PICODIR)
	cp $(BUILD)/$(PICO_GSM_CONSOLE_FILE).uf2 $(PICODIR)

# compile & write
cw: all write_main
	echo "cw main"

cwt: all write_test
	echo "cw test"

cwgsm: all write_gsm_console
	echo "cw gsm console"

clean:
	$(MAKE) clean -C $(BUILD)
	rm -rf  $(BUILD)
	mkdir $(BUILD)
	find . -name "*.s" -type f -delete
	find . -name "*.gch" -type f -delete


getFatFSlib:
	git clone https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico.git

test:
	$(MAKE) test -C $(BUILD)