#ifndef BME280_H
#define BME280_H

#include <spi.h>
#include <stddef.h>
#include <stdint.h>

/* measurement mode*/
typedef enum mode_t {
	mode_sleep  = 0x0,
	mode_normal = 0x1,
	mode_force  = 0x3
} mode_t;

/* Oversampling mode*/
typedef enum osr_t {
	osr_skip = 0x0 /*disables sensor*/,
	osr_1	 = 0x1,
	osr_2	 = 0x2,
	osr_4	 = 0x3,
	osr_8	 = 0x4,
	osr_16	 = 0x5
} osr_t;

typedef enum sb_t {
	sb_0_5ms  = 0x0,
	sb_65_5ms = 0x1,
	sb_125ms  = 0x2,
	sb_250ms  = 0x3,
	sb_500ms  = 0x4,
	sb_1000ms = 0x5,
	sb_10ms	  = 0x6,
	sb_20ms	  = 0x7,
} sb_t;

typedef enum filter_t {
	filter_off = 0x0,
	filter_2   = 0x1,
	filter_4   = 0x2,
	filter_8   = 0x3,
	filter_16  = 0x4,
} filter_t;

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

/* fails when run in normal mode */
int bme280_set_standby(bme280_p bme, sb_t standby);
/* fails when run in normal mode */
int bme280_set_filter(bme280_p bme, filter_t filter);

/* fails if coefficients are not loaded*/
int bme280_read(bme280_p bme, bme280_reads *reads);

#endif