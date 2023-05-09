
#include "bme280_types.h"

#include <stdio.h>

#define ENUM_CASE(item)                                                        \
	case item:                                                             \
		return #item

const char *mode_to_cstring(mode_t mode)
{
	switch (mode) {
		ENUM_CASE(mode_sleep);
		ENUM_CASE(mode_normal);
		ENUM_CASE(mode_force);
	};
	return "undefined";
}

const char *osr_to_cstring(osr_t osr)
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
const char *sb_to_cstring(sb_t sb)
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
const char *filter_to_cstring(filter_t filter)
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

int print_config(const config *cfg, FILE *file)
{
	return fprintf(file, "config: [t_sb: %s, filter: %s, spi3w_en: %s]",
		       sb_to_cstring(cfg->t_sb), filter_to_cstring(cfg->filter),
		       cfg->spi3w_en ? "true" : "false");
}

int print_ctrl_meas(const ctrl_meas *c_meas, FILE *file)
{
	return fprintf(file, "ctrl_meas: [osrs_t: %s, osrs_p: %s, mode: %s]",
		       osr_to_cstring(c_meas->osrs_t),
		       osr_to_cstring(c_meas->osrs_p),
		       mode_to_cstring(c_meas->mode));
}

int print_ctrl_hum(const ctrl_hum *c_hum, FILE *file)
{
	return fprintf(file, "ctrl_hum: [osrs_h: %s]",
		       osr_to_cstring(c_hum->osrs_h));
}

int print_control_registers(const control_registers *cr, FILE *file)
{
	int res = 0;
	res |= fprintf(file, "%s: [\n", __func__);

	res |= print_config(&cr->config, file);
	fprintf(file, "\n");

	res |= print_ctrl_meas(&cr->ctrl_meas, file);
	fprintf(file, "\n");
	
	res |= print_ctrl_hum(&cr->ctrl_hum, file);
	fprintf(file, "\n");

	res |= fprintf(file, "]\n");

	return res;
}
