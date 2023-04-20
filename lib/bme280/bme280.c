#include "bme280.h"
#include <bme280_access.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#define LOG() printf("%s\n", __func__)

#define BME280_CHIP_VER (0x60)
#define BME280_RESET_VALUE (0xB6)

enum bme280_addr {
	bme280_addr_chip_ver = 0xD0,
	bme280_addr_reset = 0xE0,
};
typedef enum bme280_addr bme280_addr;

struct bme280 {
	bme280_access_p acc;
};
typedef struct bme280 bme280;

bme280_p bme280_init(spi_driver *spi_drv)
{
	LOG();
	bme280_p bme = malloc(sizeof(*bme));

	if (bme) {
		bme->acc = bme280_access_init(spi_drv);
		if (!bme->acc)
			return NULL;
	}

	return bme;
}

int bme280_check_availability(bme280_p bme)
{
	LOG();
	uint8_t value;
	bme280_access_read8(bme->acc, bme280_addr_chip_ver, &value);

	return value == BME280_CHIP_VER;
}
int bme280_reset(bme280_p bme)
{
	LOG();
	bme280_access_write(bme->acc, bme280_addr_reset, BME280_RESET_VALUE);

	int retries = 10;
	int bme_available = 0;
	do {
		if (retries == 0)
			break;

		--retries;
		_delay_ms(10);
	} while (!(bme_available = bme280_check_availability(bme)));

	return bme_available;
}

void bme280_destroy(bme280_p acc)
{
	bme280_access_destroy(acc->acc);
	free(acc);
}