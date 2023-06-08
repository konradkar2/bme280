#ifndef BME280_TYPES_H
#define BME280_TYPES_H

#include <stdio.h>

/* measurement mode*/
typedef enum bme280_mode_t {
	mode_sleep  = 0x0,
	mode_force = 0x1,
	mode_normal  = 0x3,
} bme280_mode_t;

/* Oversampling mode*/
typedef enum bme280_osr_t {
	osr_skip = 0x0 /*disables sensor*/,
	osr_1	 = 0x1,
	osr_2	 = 0x2,
	osr_4	 = 0x3,
	osr_8	 = 0x4,
	osr_16	 = 0x5
} bme280_osr_t;

typedef enum bme280_sb_t {
	sb_0_5ms  = 0x0,
	sb_65_5ms = 0x1,
	sb_125ms  = 0x2,
	sb_250ms  = 0x3,
	sb_500ms  = 0x4,
	sb_1000ms = 0x5,
	sb_10ms	  = 0x6,
	sb_20ms	  = 0x7,
} bme280_sb_t;

typedef enum bme280_filter_t {
	filter_off = 0x0,
	filter_2   = 0x1,
	filter_4   = 0x2,
	filter_8   = 0x3,
	filter_16  = 0x4,
} bme280_filter_t;

struct bme280_osr_settings {
	bme280_osr_t temp;
	bme280_osr_t press;
	bme280_osr_t hum;
};
typedef struct bme280_osr_settings bme280_osr_settings;

struct bme280_reads {
	int32_t temperature;
	uint32_t pressure;
	uint32_t humidity;
};
typedef struct bme280_reads bme280_reads;

struct bme280_coefficients {
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
typedef struct bme280_coefficients bme280_coefficients;

typedef union bme280_config {
	uint8_t v;
	struct {
		uint8_t spi3w_en : 1;
		uint8_t reserved : 1;
		uint8_t filter : 3;
		uint8_t t_sb : 3;
	};
} bme280_config;

typedef union bme280_ctrl_meas {
	uint8_t v;
	struct {
		uint8_t mode : 2;
		uint8_t osrs_p : 3;
		uint8_t osrs_t : 3;
	};
} bme280_ctrl_meas;

typedef union bme280_ctrl_hum {
	uint8_t v;
	struct {
		uint8_t osrs_h : 3;
		uint8_t reserved : 5;
	};
} bme280_ctrl_hum;

struct bme280_control_registers {
	bme280_config bme280_config;
	bme280_ctrl_meas bme280_ctrl_meas;
	bme280_ctrl_hum bme280_ctrl_hum;
};
typedef struct bme280_control_registers bme280_control_registers;

int bme280_print_config(const bme280_config *cfg, FILE *file);
int bme280_print_ctrl_meas(const bme280_ctrl_meas *c_meas, FILE *file);
int bme280_print_ctrl_hum(const bme280_ctrl_hum *c_hum, FILE *file);

int bme280_print_control_registers(const bme280_control_registers *cr, FILE *file);

const char *bme280_mode_to_cstring(bme280_mode_t mode);
const char *bme280_osr_to_cstring(bme280_osr_t osr);
const char *bme280_sb_to_cstring(bme280_sb_t sb);
const char *bme280_filter_to_cstring(bme280_filter_t filter);

#endif