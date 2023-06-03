#ifndef BME280_TYPES_H
#define BME280_TYPES_H

#include <stdio.h>

/* measurement mode*/
typedef enum mode_t {
	mode_sleep  = 0x0,
	mode_force = 0x1,
	mode_normal  = 0x3,
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

struct bme280_reads {
	int32_t temperature;
	uint32_t pressure;
	uint32_t humidity;
};
typedef struct bme280_reads bme280_reads;

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

typedef union config {
	uint8_t v;
	struct {
		uint8_t spi3w_en : 1;
		uint8_t reserved : 1;
		uint8_t filter : 3;
		uint8_t t_sb : 3;
	};
} config;

typedef union ctrl_meas {
	uint8_t v;
	struct {
		uint8_t mode : 2;
		uint8_t osrs_p : 3;
		uint8_t osrs_t : 3;
	};
} ctrl_meas;

typedef union ctrl_hum {
	uint8_t v;
	struct {
		uint8_t osrs_h : 3;
		uint8_t reserved : 5;
	};
} ctrl_hum;

struct control_registers {
	config config;
	ctrl_meas ctrl_meas;
	ctrl_hum ctrl_hum;
};
typedef struct control_registers control_registers;

int print_config(const config *cfg, FILE *file);
int print_ctrl_meas(const ctrl_meas *c_meas, FILE *file);
int print_ctrl_hum(const ctrl_hum *c_hum, FILE *file);

int print_control_registers(const control_registers *cr, FILE *file);

const char *mode_to_cstring(mode_t mode);
const char *osr_to_cstring(osr_t osr);
const char *sb_to_cstring(sb_t sb);
const char *filter_to_cstring(filter_t filter);

#endif