#include "bme280.h"
#include <assert.h>
#include <bme280_access.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#define LOG() printf("%s\n", __func__)

#define BME280_CHIP_VER (0x60)
#define BME280_RESET_VALUE (0xB6)

struct coefficients {
	/*temperature*/
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	/* pressure*/
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
	/* humidity*/
	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;
};
typedef struct coefficients coefficients;

struct bme280 {
	bme280_access_p acc;
	coefficients *coeffs;
	bool coeffs_loaded;
};
typedef struct bme280 bme280;

bme280_p bme280_init(spi_driver *spi_drv)
{
	LOG();
	bme280_p bme = malloc(sizeof(*bme));
	bme->coeffs_loaded = false;

	if (bme) {
		bme->acc = bme280_access_init(spi_drv);
		if (!bme->acc) {
			bme280_destroy(bme);
			return NULL;
		}
		bme->coeffs = calloc(1, sizeof(coefficients));
		if (!bme->coeffs) {
			bme280_destroy(bme);
			return NULL;
		}
	}

	return bme;
}

void bme280_destroy(bme280_p acc)
{
	if (acc) {
		bme280_access_destroy(acc->acc);
		free(acc->coeffs);
		free(acc);
	}
}

int bme280_check_availability(bme280_p bme)
{
	LOG();
	uint8_t value;
	bme280_access_read8(bme->acc, addr_chip_ver, &value);

	return value == BME280_CHIP_VER;
}
int bme280_reset(bme280_p bme)
{
	LOG();
	bme280_access_write(bme->acc, addr_reset, BME280_RESET_VALUE);

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

void bme280_load_coefficients(bme280_p b)
{
	uint8_t c[42];
	memset(c, 0, sizeof(c));

	bme280_access_read_n(b->acc, addr_calib0_25, 26, c);
	bme280_access_read_n(b->acc, addr_calib26_41, 16, c + 26);

	size_t idx = 0;
	b->coeffs->dig_T1 = (c[idx + 1] << 8) | c[idx];
	idx += 2;

	b->coeffs->dig_T2 = (c[idx + 1] << 8) | c[idx];
	idx += 2;

	b->coeffs->dig_T3 = (c[idx + 1] << 8) | c[idx];
	idx += 2;

	b->coeffs->dig_P1 = (c[idx + 1] << 8) | c[idx];
	idx += 2;
	b->coeffs->dig_P2 = (c[idx + 1] << 8) | c[idx];
	idx += 2;
	b->coeffs->dig_P3 = (c[idx + 1] << 8) | c[idx];
	idx += 2;
	b->coeffs->dig_P4 = (c[idx + 1] << 8) | c[idx];
	idx += 2;
	b->coeffs->dig_P5 = (c[idx + 1] << 8) | c[idx];
	idx += 2;
	b->coeffs->dig_P6 = (c[idx + 1] << 8) | c[idx];
	idx += 2;
	b->coeffs->dig_P7 = (c[idx + 1] << 8) | c[idx];
	idx += 2;
	b->coeffs->dig_P8 = (c[idx + 1] << 8) | c[idx];
	idx += 2;
	b->coeffs->dig_P9 = (c[idx + 1] << 8) | c[idx];
	idx += 2;

	b->coeffs->dig_H1 = c[idx];
	idx += 1;
	b->coeffs->dig_H2 = (c[idx + 1] << 8) | c[idx];
	idx += 2;
	b->coeffs->dig_H3 = c[idx];
	idx += 1;

	b->coeffs->dig_H4 = c[idx] << 4;
	idx += 1;
	b->coeffs->dig_H4 |= (c[idx] & 0b00001111);

	b->coeffs->dig_H5 = (c[idx + 1] << 4) | c[idx] >> 4;
	idx += 1;
	b->coeffs->dig_H6 |= c[idx];

	b->coeffs_loaded = true;
}