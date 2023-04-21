#ifndef BME280_H
#define BME280_H

#include <spi.h>
#include <stddef.h>
#include <stdint.h>

typedef struct bme280 *bme280_p;

bme280_p bme280_init(spi_driver *spi_drv);
void bme280_destroy(bme280_p acc);

int bme280_check_availability(bme280_p bme);
int bme280_reset(bme280_p bme);
void bme280_load_coefficients(bme280_p bme);

#endif