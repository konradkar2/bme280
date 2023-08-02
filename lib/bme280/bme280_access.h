#ifndef BME280_ACCESS_H
#define BME280_ACCESS_H

#include <i2c.h>
#include <spi.h>
#include <stddef.h>
#include <stdint.h>

enum bme280_addr {
	addr_chip_ver	= 0xD0,
	addr_reset	= 0xE0,
	addr_calib0_25	= 0x88,
	addr_calib26_41 = 0xE1,
	addr_config	= 0xF5,
	addr_ctrl_meas	= 0xF4,
	addr_status	= 0xF3,
	addr_ctrl_hum	= 0xF2,
	addr_press_msb	= 0xF7,
	addr_hum_lsb	= 0xFE,
};
typedef enum bme280_addr bme280_addr;

typedef struct bme280_access bme280_access;

bme280_access *bme280_access_init_spi(spi_driver *spi_drv);
bme280_access *bme280_access_init_i2c(i2c_driver *i2c_drv);
void bme280_access_destroy(bme280_access *acc);

int8_t bme280_access_read8(bme280_access *acc, bme280_addr addr, uint8_t *out);
int8_t bme280_access_read_n(bme280_access *acc, bme280_addr addr, size_t n,
			  uint8_t *out);
int8_t bme280_access_write(bme280_access *acc, bme280_addr addr, uint8_t value);

#endif