#ifndef BME280_H
#define BME280_H

#include <spi.h>
#include <stddef.h>
#include <stdint.h>

typedef enum mode_t { mode_sleep, mode_normal, mode_force } mode_t;
typedef enum osr_t {
	osr_skip /*disables sensor*/,
	osr_1,
	osr_2,
	osr_4,
	osr_8,
	osr_16
} osr_t;

struct bme280_osr_settings {
	osr_t temp;
	osr_t press;
	osr_t hum;
};
typedef struct bme280_osr_settings bme280_osr_settings;

typedef struct bme280 *bme280_p;

struct bme280_reads {
	int temperature;
	int pressure;
	int humidity;
};
typedef struct bme280_reads bme280_reads;

bme280_p bme280_init(spi_driver *spi_drv);
void bme280_destroy(bme280_p acc);

int bme280_check_availability(bme280_p bme);
int bme280_reset(bme280_p bme);
void bme280_load_coefficients(bme280_p bme);
void bme280_load_control_registers(bme280_p bme);
void bme280_set_mode(bme280_p bme, mode_t mode);
void bme280_set_osr_settings(bme280_p bme, bme280_osr_settings osr_settings);

bme280_reads bme280_read(bme280_p bme);

#endif