PICODIR := ./pico #/mnt/h
BUILD := build
PICO_MAIN_FILE := pico_main
PICO_TEST_FILE := pico_test


all:
	$(MAKE) -C $(BUILD) -j 16

cmake:
	cmake -S ./ -B $(BUILD) -DPICO_MAIN_FILE=$(PICO_MAIN_FILE) -DPICO_TEST_FILE=$(PICO_TEST_FILE)

w: write_main
write_main:
	mount -t drvfs H: $(PICODIR)
	cp $(BUILD)/$(PICO_MAIN_FILE).uf2 $(PICODIR)

write_test:
	mount -t drvfs H: $(PICODIR)
	cp $(BUILD)/$(PICO_TEST_FILE).uf2 $(PICODIR)

# compile & write
cw: all write_main
	echo "cw main"

cwt: all write_test
	echo "cw test"

clean:
	$(MAKE) clean -C $(build)