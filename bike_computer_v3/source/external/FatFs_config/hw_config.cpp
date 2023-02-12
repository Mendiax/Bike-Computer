#include <string.h>
#include "my_debug.h"
#include "hw_config.h"
#include "ff.h"
#include "diskio.h"

#ifdef __cplusplus
extern "C" {
#endif

static spi_t spis[] = {
    {
        .hw_inst = spi0,
        .miso_gpio = 4,
        .mosi_gpio = 7,
        .sck_gpio = 6,

        .baud_rate = 1000 * 1000,

        .set_drive_strength = true,
        .mosi_gpio_drive_strength = GPIO_DRIVE_STRENGTH_2MA,
        .sck_gpio_drive_strength = GPIO_DRIVE_STRENGTH_2MA,
    }
};

static sd_card_t sd_cards[] = {
    {
        .pcName = "0:",           // Name used to mount device
        .spi = &spis[0],          // Pointer to the SPI driving this card
        .ss_gpio = 5,             // The SPI slave select GPIO for this SD card

        .use_card_detect = false,
        .card_detect_gpio = 22,   // Card detect
        .card_detected_true = 1,  // What the GPIO read returns when a card is
                                  // present. Use -1 if there is no card detect.
        .set_drive_strength = true,
        .ss_gpio_drive_strength = GPIO_DRIVE_STRENGTH_2MA,
        .m_Status = STA_NOINIT,
    }
};

/* ********************************************************************** */
size_t sd_get_num() { return count_of(sd_cards); }
sd_card_t *sd_get_by_num(size_t num) {
    if (num <= sd_get_num()) {
        return &sd_cards[num];
    } else {
        return NULL;
    }
}
size_t spi_get_num() { return count_of(spis); }
spi_t *spi_get_by_num(size_t num) {
    if (num <= sd_get_num()) {
        return &spis[num];
    } else {
        return NULL;
    }
}

#ifdef __cplusplus
}
#endif

