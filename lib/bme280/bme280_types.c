
#include "bme280_types.h"

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

int describe_config(const config *cfg, char *buff, size_t buffSize)
{
	return snprintf(
	    buff, buffSize, "config: [t_sb: %s, filter: %s, spi3w_en: %s]",
	    sb_to_cstring(cfg->t_sb), filter_to_cstring(cfg->filter),
	    cfg->spi3w_en ? "true" : "false");
}

int describe_ctrl_meas(const ctrl_meas *c_meas, char *buff, size_t buffSize)
{
	return snprintf(
	    buff, buffSize, "ctrl_meas: [osrs_t: %s, osrs_p: %s, mode: %s]",
	    osr_to_cstring(c_meas->osrs_t), osr_to_cstring(c_meas->osrs_p),
	    mode_to_cstring(c_meas->mode));
}

int describe_ctrl_hum(const ctrl_hum *c_hum, char *buff, size_t buffSize)
{
	return snprintf(buff, buffSize, "ctrl_hum: [osrs_h: %s]",
			osr_to_cstring(c_hum->osrs_h));
}

int describe_control_registers(const control_registers *cr, char *buff,
			       size_t buffSize)
{
	char buff_config[100]	 = {0};
	char buff_ctrl_meas[100] = {0};
	char buff_ctrl_hum[100]	 = {0};

	int len;
	len = describe_config(&cr->config, buff_config, sizeof buff_config);
	if (len < 0 || len >= (int)sizeof buff_config) {
		return -1;
	}

	len = describe_ctrl_meas(&cr->ctrl_meas, buff_ctrl_meas,
				 sizeof buff_ctrl_meas);
	if (len < 0 || len >= (int)sizeof buff_ctrl_meas) {
		return -1;
	}

	len = describe_ctrl_hum(&cr->ctrl_hum, buff_ctrl_hum,
				sizeof buff_ctrl_hum);
	if (len < 0 || len >= (int)sizeof buff_ctrl_hum) {
		return -1;
	}

	return snprintf(buff, buffSize, "control registers: [%s, %s, %s]",
			buff_config, buff_ctrl_meas, buff_ctrl_hum);
}

int print_control_registers(const control_registers *cr)
{
	int ret;
	char buff[500] = {0};

	ret = describe_control_registers(cr, buff, sizeof buff);
	if (ret < 0 || ret >= (int)sizeof buff) {
		return -1;
	}

	return printf("%s\n", buff);
}
