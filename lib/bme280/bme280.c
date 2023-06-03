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
#define LOG_BYTE(value)                                                        \
	printf("%s: %s: 0x%02x\n", __func__, #value, (unsigned)value)

#define BME280_CHIP_VER (0x60)
#define BME280_RESET_VALUE (0xB6)

struct bme280 {
	bme280_access_p acc;
	coefficients *coeffs;
	bool coeffs_loaded;
	int32_t t_fine;
	control_registers ctrl_regs;
};
typedef struct bme280 bme280;

bme280_p bme280_init(spi_driver *spi_drv)
{
	LOG();
	bme280_p bme = malloc(sizeof(*bme));

	if (bme) {
		// memset(&bme->ctrl_regs, 0, sizeof(control_registers));
		bme->coeffs_loaded = false;
		bme->acc	   = bme280_access_init(spi_drv);
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

void bme280_destroy(bme280_p bme)
{
	if (bme) {
		bme280_access_destroy(bme->acc);
		free(bme->coeffs);
		free(bme);
	}
}

int bme280_is_available(bme280_p bme)
{
	LOG();
	uint8_t value;
	bme280_access_read8(bme->acc, addr_chip_ver, &value);

	return value == BME280_CHIP_VER;
}

int bme280_reset(bme280_p bme)
{
	LOG();
	bme280_set_mode(bme, mode_sleep);
	bme280_access_write(bme->acc, addr_reset, BME280_RESET_VALUE);

	_delay_ms(15);

	if (!bme280_is_available(bme)) {
		printf("bme280 not available");
		return 1;
	}

	uint8_t status = 0x01;
	while (status & 0x01) {
		bme280_access_read8(bme->acc, addr_status, &status);
		LOG_BYTE(status);
	}

	return 0;
}

static size_t coeff_addr_to_idx(uint8_t addr)
{
	if (addr <= 0xA1)
		return addr - 0x88;
	else if (addr <= 0xE7)
		return (addr - 0xE1) + 26;
	else {
		fprintf(stderr, "%s: Mismatch between addr and idx", __func__);
		exit(-1);
	}
}

static void bme280_apply_ctrl_regs(bme280_p bme)
{
	bme280_set_mode(bme, mode_sleep);
	/* BME280_datasheet - changing ctrl_meas has to be done
	after a change to ctrl_hum */
	bme280_access_write(bme->acc, addr_ctrl_hum, bme->ctrl_regs.ctrl_hum.v);
	bme280_access_write(bme->acc, addr_ctrl_meas,
			    bme->ctrl_regs.ctrl_meas.v);
	bme280_access_write(bme->acc, addr_config, bme->ctrl_regs.config.v);
}

void bme280_set_mode(bme280_p bme, mode_t mode)
{
	LOG();
	bme->ctrl_regs.ctrl_meas.mode = mode;

	/* BME280_datasheet - changing ctrl_meas has to be done
	after a change to ctrl_hum */
	bme280_access_write(bme->acc, addr_ctrl_hum, bme->ctrl_regs.ctrl_hum.v);
	bme280_access_write(bme->acc, addr_ctrl_meas,
			    bme->ctrl_regs.ctrl_meas.v);
	_delay_ms(1);
}

int bme280_set_standby(bme280_p bme, sb_t standby)
{
	LOG();
	if (bme->ctrl_regs.ctrl_meas.mode == mode_normal) {
		return 1;
	}

	bme->ctrl_regs.config.t_sb = standby;
	bme280_apply_ctrl_regs(bme);

	return 0;
}

int bme280_set_filter(bme280_p bme, filter_t filter)
{
	LOG();
	if (bme->ctrl_regs.ctrl_meas.mode == mode_normal) {
		return 1;
	}

	bme->ctrl_regs.config.filter = filter;
	bme280_apply_ctrl_regs(bme);

	return 0;
}

void bme280_set_osr_settings(bme280 *bme, bme280_osr_settings osr_settings)
{
	LOG();
	bme->ctrl_regs.ctrl_meas.osrs_t = osr_settings.temp;
	bme->ctrl_regs.ctrl_meas.osrs_p = osr_settings.press;
	bme->ctrl_regs.ctrl_hum.osrs_h	= osr_settings.hum;

	bme280_apply_ctrl_regs(bme);
}

void bme280_load_control_registers(bme280_p bme)
{
	LOG();
	uint8_t regs[4];
	bme280_access_read_n(bme->acc, addr_ctrl_hum, sizeof regs, regs);
	bme->ctrl_regs.ctrl_hum.v = regs[0];
	// skip status register (regs[1])
	bme->ctrl_regs.ctrl_meas.v = regs[2];
	bme->ctrl_regs.config.v	   = regs[3];

	print_control_registers(&bme->ctrl_regs, stdout);
}

void bme280_read_control_registers(bme280_p bme, FILE *file)
{
	LOG();
	uint8_t regs[4];
	bme280_access_read_n(bme->acc, addr_ctrl_hum, sizeof regs, regs);

	control_registers cr;
	cr.ctrl_hum.v = regs[0];
	// skip status register (regs[1])
	cr.ctrl_meas.v = regs[2];
	cr.config.v    = regs[3];

	print_control_registers(&cr, file);
}

#define COEFF_IDX(addr) coeff_addr_to_idx(addr)

void bme280_load_coefficients(bme280_p b)
{
	LOG();

	uint8_t c[42];
	bme280_access_read_n(b->acc, addr_calib0_25, 26, c);
	bme280_access_read_n(b->acc, addr_calib26_41, 16, c + 26);

	b->coeffs->dig_T1 =
	    ((uint16_t)c[COEFF_IDX(0x89)] << 8) | c[COEFF_IDX(0x88)];
	b->coeffs->dig_T2 =
	    ((int16_t)c[COEFF_IDX(0x8B)] << 8) | c[COEFF_IDX(0x8A)];
	b->coeffs->dig_T3 =
	    ((int16_t)c[COEFF_IDX(0x8D)] << 8) | c[COEFF_IDX(0x8C)];

	b->coeffs->dig_P1 =
	    ((uint16_t)c[COEFF_IDX(0x8F)] << 8) | c[COEFF_IDX(0x8E)];
	b->coeffs->dig_P2 =
	    ((int16_t)c[COEFF_IDX(0x91)] << 8) | c[COEFF_IDX(0x90)];
	b->coeffs->dig_P3 =
	    ((int16_t)c[COEFF_IDX(0x93)] << 8) | c[COEFF_IDX(0x92)];
	b->coeffs->dig_P4 =
	    ((int16_t)c[COEFF_IDX(0x95)] << 8) | c[COEFF_IDX(0x94)];
	b->coeffs->dig_P5 =
	    ((int16_t)c[COEFF_IDX(0x97)] << 8) | c[COEFF_IDX(0x96)];
	b->coeffs->dig_P6 =
	    ((int16_t)c[COEFF_IDX(0x99)] << 8) | c[COEFF_IDX(0x98)];
	b->coeffs->dig_P7 =
	    ((int16_t)c[COEFF_IDX(0x9B)] << 8) | c[COEFF_IDX(0x9A)];
	b->coeffs->dig_P8 =
	    ((int16_t)c[COEFF_IDX(0x9D)] << 8) | c[COEFF_IDX(0x9C)];
	b->coeffs->dig_P9 =
	    ((int16_t)c[COEFF_IDX(0x9F)] << 8) | c[COEFF_IDX(0x9E)];

	b->coeffs->dig_H1 = (uint8_t)c[COEFF_IDX(0xA1)];
	b->coeffs->dig_H2 =
	    ((int16_t)c[COEFF_IDX(0xE2)] << 8) | c[COEFF_IDX(0xE1)];
	b->coeffs->dig_H3 = (uint8_t)c[COEFF_IDX(0xE3)];
	b->coeffs->dig_H4 =
	    ((int16_t)c[COEFF_IDX(0xE4)] << 4) | (c[COEFF_IDX(0xE5)] & 0x0F);
	b->coeffs->dig_H5 =
	    ((int16_t)c[COEFF_IDX(0xE6)] << 4) | c[COEFF_IDX(0xE5)] >> 4;
	b->coeffs->dig_H6 = (int8_t)c[COEFF_IDX(0xE7)];

	b->coeffs_loaded = true;
}

typedef int32_t BME280_S32_t;
typedef uint32_t BME280_U32_t;
typedef int64_t BME280_S64_t;

BME280_S32_t BME280_compensate_T_int32(bme280 *drv, BME280_S32_t adc_T)
{
	BME280_S32_t var1, var2, T;
	var1 = ((((adc_T >> 3) - ((BME280_S32_t)drv->coeffs->dig_T1 << 1))) *
		((BME280_S32_t)drv->coeffs->dig_T2)) >>
	       11;
	var2 = (((((adc_T >> 4) - ((BME280_S32_t)drv->coeffs->dig_T1)) *
		  ((adc_T >> 4) - ((BME280_S32_t)drv->coeffs->dig_T1))) >>
		 12) *
		((BME280_S32_t)drv->coeffs->dig_T3)) >>
	       14;
	drv->t_fine = var1 + var2;
	T	    = (drv->t_fine * 5 + 128) >> 8;
	return T;
}

// Q24.8
static BME280_U32_t BME280_compensate_P_int64(bme280 *drv, BME280_S32_t adc_P)
{
	BME280_S64_t var1, var2, p;
	var1 = ((BME280_S64_t)drv->t_fine) - 128000;
	var2 = var1 * var1 * (BME280_S64_t)drv->coeffs->dig_P6;
	var2 = var2 + ((var1 * (BME280_S64_t)drv->coeffs->dig_P5) << 17);
	var2 = var2 + (((BME280_S64_t)drv->coeffs->dig_P4) << 35);
	var1 = ((var1 * var1 * (BME280_S64_t)drv->coeffs->dig_P3) >> 8) +
	       ((var1 * (BME280_S64_t)drv->coeffs->dig_P2) << 12);
	var1 = (((((BME280_S64_t)1) << 47) + var1)) *
		   ((BME280_S64_t)drv->coeffs->dig_P1) >>
	       33;
	if (var1 == 0) {
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576 - adc_P;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 =
	    (((BME280_S64_t)drv->coeffs->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((BME280_S64_t)drv->coeffs->dig_P8) * p) >> 19;
	p    = ((p + var1 + var2) >> 8) +
	    (((BME280_S64_t)drv->coeffs->dig_P7) << 4);
	return (BME280_U32_t)p;
}

// return humidity in Q22.10
static BME280_U32_t bme280_compensate_H_int32(bme280 *drv, BME280_S32_t adc_H)
{
	BME280_S32_t v_x1_u32r;
	v_x1_u32r = (drv->t_fine - ((BME280_S32_t)76800));
	v_x1_u32r =
	    (((((adc_H << 14) - (((BME280_S32_t)drv->coeffs->dig_H4) << 20) -
		(((BME280_S32_t)drv->coeffs->dig_H5) * v_x1_u32r)) +
	       ((BME280_S32_t)16384)) >>
	      15) *
	     (((((((v_x1_u32r * ((BME280_S32_t)drv->coeffs->dig_H6)) >> 10) *
		  (((v_x1_u32r * ((BME280_S32_t)drv->coeffs->dig_H3)) >> 11) +
		   ((BME280_S32_t)32768))) >>
		 10) +
		((BME280_S32_t)2097152)) *
		   ((BME280_S32_t)drv->coeffs->dig_H2) +
	       8192) >>
	      14));
	v_x1_u32r =
	    (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
			   ((BME280_S32_t)drv->coeffs->dig_H1)) >>
			  4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (BME280_U32_t)(v_x1_u32r >> 12);
}

int bme280_read(bme280_p bme, bme280_reads *reads)
{
	if (!bme->coeffs_loaded) {
		return 1;
	}

	size_t reads_size = 8; // (addr_hum_lsb - addr_press_msb) + 1;
	uint8_t read_raw[reads_size];
	bme280_access_read_n(bme->acc, addr_press_msb, reads_size, read_raw);

	uint32_t adc_press = 0, adc_temp = 0, adc_hum = 0;
	adc_press = ((uint32_t)read_raw[0] << 12) |
		    ((uint32_t)read_raw[1] << 4) | ((uint32_t)read_raw[2] >> 4);
	adc_temp = ((uint32_t)read_raw[3] << 12) |
		   ((uint32_t)read_raw[4] << 4) | ((uint32_t)read_raw[5] >> 4);
	adc_hum = ((uint32_t)read_raw[6] << 8) | ((uint32_t)read_raw[7] << 4);

	reads->pressure	   = BME280_compensate_P_int64(bme, adc_press) / 256;
	reads->temperature = BME280_compensate_T_int32(bme, adc_temp) / 100;
	reads->humidity	   = bme280_compensate_H_int32(bme, adc_hum) / 1024;

	return 0;
}