#ifndef BME280_H
#define BME280_H

#include <spi.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "bme280_types.h"
#include "bme280_access.h"

typedef struct bme280 *bme280_p;

bme280_p bme280_init(bme280_access * access);
void bme280_destroy(bme280_p acc);

int8_t bme280_is_available(bme280_p bme);
int8_t bme280_reset(bme280_p bme);
int8_t bme280_load_coefficients(bme280_p bme);
int8_t bme280_load_control_registers(bme280_p bme);
int8_t bme280_get_control_registers(bme280_p bme, bme280_control_registers *cr);
int8_t bme280_set_mode(bme280_p bme, bme280_mode_t mode);
int8_t bme280_set_osr_settings(bme280_p bme, bme280_osr_settings osr_settings);

int8_t bme280_set_standby(bme280_p bme, bme280_sb_t standby);
int8_t bme280_set_filter(bme280_p bme, bme280_filter_t filter);

/* fails if coefficients are not loaded*/
int8_t bme280_read(bme280_p bme, bme280_reads *reads);

#endif