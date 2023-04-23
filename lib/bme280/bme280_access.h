#ifndef BME280_ACCESS_H
#define BME280_ACCESS_H

#include <spi.h>
#include <stddef.h>
#include <stdint.h>

enum bme280_addr {
	addr_chip_ver = 0xD0,
	addr_reset = 0xE0,
	addr_calib0_25 = 0x88,
	addr_calib26_41 = 0xE1,
	addr_config = 0xF5,
	addr_ctrl_meas = 0xF4,
	addr_ctrl_hum = 0xF2,
	addr_press_msb = 0xF7,
	addr_hum_lsb = 0xFE,
};
typedef enum bme280_addr bme280_addr;

typedef struct bme280_access *bme280_access_p;

bme280_access_p bme280_access_init(spi_driver *spi_drv);
void bme280_access_destroy(bme280_access_p acc);

void bme280_access_read8(bme280_access_p acc, bme280_addr addr, uint8_t *out);
void bme280_access_read16(bme280_access_p acc, bme280_addr addr, uint16_t *out);
void bme280_access_read_n(bme280_access_p acc, bme280_addr addr, size_t n,
			  uint8_t *out);
void bme280_access_write(bme280_access_p acc, bme280_addr addr, uint8_t value);

#endif