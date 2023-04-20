#ifndef BME280_ACCESS_H
#define BME280_ACCESS_H

#include <spi.h>
#include <stddef.h>
#include <stdint.h>

typedef struct bme280_access *bme280_access_p;

bme280_access_p bme280_access_init(spi_driver *spi_drv);
void bme280_access_destroy(bme280_access_p acc);

void bme280_access_read8(bme280_access_p acc, uint8_t addr, uint8_t *out);
void bme280_access_read16(bme280_access_p acc, uint8_t addr, uint16_t *out);
void bme280_access_read_n(bme280_access_p acc, uint8_t addr, uint8_t *out,
			  size_t n);
void bme280_access_write(bme280_access_p acc, uint8_t addr, uint8_t value);

#endif