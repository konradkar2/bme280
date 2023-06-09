
#include "bme280_types.h"

#include <stdio.h>

#define ENUM_CASE(item)                                                        \
	case item:                                                             \
		return #item

const char *bme280_mode_to_cstring(bme280_mode_t mode)
{
	switch (mode) {
		ENUM_CASE(mode_sleep);
		ENUM_CASE(mode_normal);
		ENUM_CASE(mode_force);
	};
	return "undefined";
}

const char *bme280_osr_to_cstring(bme280_osr_t osr)
{
	switch (osr) {
		ENUM_CASE(osr_skip);
		ENUM_CASE(osr_1);
		ENUM_CASE(osr_2);
		ENUM_CASE(osr_4);
		ENUM_CASE(osr_8);
		ENUM_CASE(osr_16);
	};
	return "undefined";
}
const char *bme280_sb_to_cstring(bme280_sb_t sb)
{
	switch (sb) {
		ENUM_CASE(sb_0_5ms);
		ENUM_CASE(sb_65_5ms);
		ENUM_CASE(sb_125ms);
		ENUM_CASE(sb_250ms);
		ENUM_CASE(sb_500ms);
		ENUM_CASE(sb_1000ms);
		ENUM_CASE(sb_10ms);
		ENUM_CASE(sb_20ms);
	};
	return "undefined";
}
const char *bme280_filter_to_cstring(bme280_filter_t filter)
{
	switch (filter) {
		ENUM_CASE(filter_off);
		ENUM_CASE(filter_2);
		ENUM_CASE(filter_4);
		ENUM_CASE(filter_8);
		ENUM_CASE(filter_16);
	};
	return "undefined";
}

int bme280_print_config(const bme280_config *cfg, FILE *file)
{
	return fprintf(file, "bme280_config: [t_sb: %s, filter: %s, spi3w_en: %s]",
		       bme280_sb_to_cstring(cfg->t_sb), bme280_filter_to_cstring(cfg->filter),
		       cfg->spi3w_en ? "true" : "false");
}

int bme280_print_ctrl_meas(const bme280_ctrl_meas *c_meas, FILE *file)
{
	return fprintf(file, "bme280_ctrl_meas: [osrs_t: %s, osrs_p: %s, mode: %s]",
		       bme280_osr_to_cstring(c_meas->osrs_t),
		       bme280_osr_to_cstring(c_meas->osrs_p),
		       bme280_mode_to_cstring(c_meas->mode));
}

int bme280_print_ctrl_hum(const bme280_ctrl_hum *c_hum, FILE *file)
{
	return fprintf(file, "bme280_ctrl_hum: [osrs_h: %s]",
		       bme280_osr_to_cstring(c_hum->osrs_h));
}

int bme280_print_control_registers(const bme280_control_registers *cr, FILE *file)
{
	int res = 0;
	res |= fprintf(file, "%s: [\n", __func__);

	res |= bme280_print_config(&cr->bme280_config, file);
	fprintf(file, "\n");

	res |= bme280_print_ctrl_meas(&cr->bme280_ctrl_meas, file);
	fprintf(file, "\n");
	
	res |= bme280_print_ctrl_hum(&cr->bme280_ctrl_hum, file);
	fprintf(file, "\n");

	res |= fprintf(file, "]");
	res |= fprintf(file, "\n");

	return res;
}
