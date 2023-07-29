#include "bme280_access.h"
#include <spi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// sets msb bit to 1
#define READ_ADDR(addr) (addr | _BV(7))

// sets msb bit to 0
#define WRITE_ADDR(addr) (addr & ~(_BV(7)))

#define LOG() printf("%s\n", __func__)
#define ASSERT_I2C_STATUS(STATUS)                                              \
	if (STATUS) {                                                          \
		printf("%s: I2C error, status: 0x%02x\n", __func__, STATUS);   \
		return;                                                        \
	}

struct bme280_access {
	spi_driver *spi_drv;
	i2c_driver *i2c_drv;
};
typedef struct bme280_access bme280_access;

bme280_access *bme280_access_init_spi(spi_driver *spi_drv)
{
	LOG();
	bme280_access *acc = calloc(1, sizeof(*acc));

	if (acc) {
		acc->spi_drv = spi_drv;
		spi_driver_init_master(acc->spi_drv, spi_mode_0, msb_first);
	}

	return acc;
}

bme280_access *bme280_access_init_i2c(i2c_driver *i2c_drv)
{
	LOG();
	bme280_access *acc = calloc(1, sizeof(*acc));

	if (acc) {
		acc->i2c_drv = i2c_drv;
	}
	return acc;
}

void bme280_access_destroy(bme280_access *acc)
{
	LOG();
	if (acc) {
		free(acc);
	}
}

void bme280_access_read8(bme280_access *acc, bme280_addr addr, uint8_t *out)
{
	LOG();
	bme280_access_read_n(acc, addr, 1, out);
}

void bme280_access_read_n(bme280_access *acc, bme280_addr addr, size_t n,
			  uint8_t *out)
{
	LOG();

	if (acc->spi_drv) {
		spi_driver *spi_drv = acc->spi_drv;
		spi_driver_start(spi_drv);
		spi_driver_transmit(READ_ADDR(addr));
		for (size_t i = 0; i < n; ++i) {
			spi_driver_transmit(READ_ADDR(addr));
			*out = spi_driver_read();
			out++;
		}
		spi_driver_stop(spi_drv);
	} else {
		i2c_driver *i2c_drv = acc->i2c_drv;

		int status = 0;
		status	   = i2c_driver_start(i2c_drv);
		ASSERT_I2C_STATUS(status);

		status = i2c_driver_transmit(READ_ADDR(addr));
		ASSERT_I2C_STATUS(status);

		status = i2c_driver_read(i2c_drv, n, out);
		ASSERT_I2C_STATUS(status);

		status = i2c_driver_stop();
		ASSERT_I2C_STATUS(status);
	}
}

void bme280_access_write(bme280_access *acc, bme280_addr addr,
			 const uint8_t value)
{
	if (acc->spi_drv) {
		spi_driver_start(acc->spi_drv);
		spi_driver_transmit(WRITE_ADDR(addr));
		spi_driver_transmit(value);
		spi_driver_stop(acc->spi_drv);
	} else {
		i2c_driver *i2c_drv = acc->i2c_drv;

		int status = 0;
		status	   = i2c_driver_start(i2c_drv);
		ASSERT_I2C_STATUS(status);

		status = i2c_driver_transmit(WRITE_ADDR(addr));
		ASSERT_I2C_STATUS(status);

		status = i2c_driver_transmit(value);
		ASSERT_I2C_STATUS(status);
	}
}