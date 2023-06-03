#ifndef BME280_H
#define BME280_H

#include <spi.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "bme280_types.h"

typedef struct bme280 *bme280_p;

bme280_p bme280_init(spi_driver *spi_drv);
void bme280_destroy(bme280_p acc);

int bme280_is_available(bme280_p bme);
int bme280_reset(bme280_p bme);
void bme280_load_coefficients(bme280_p bme);
void bme280_load_control_registers(bme280_p bme);
void bme280_read_control_registers(bme280_p bme, FILE * file);
void bme280_set_mode(bme280_p bme, mode_t mode);
void bme280_set_osr_settings(bme280_p bme, bme280_osr_settings osr_settings);

/* fails when run in normal mode */
int bme280_set_standby(bme280_p bme, sb_t standby);
/* fails when run in normal mode */
int bme280_set_filter(bme280_p bme, filter_t filter);

/* fails if coefficients are not loaded*/
int bme280_read(bme280_p bme, bme280_reads *reads);

#endif