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

struct bme280_access {
	spi_driver *spi_drv;
};
typedef struct bme280_access bme280_access;

bme280_access_p bme280_access_init(spi_driver *spi_drv)
{
	LOG();
	bme280_access_p acc = malloc(sizeof(*acc));

	if (acc) {
		acc->spi_drv = spi_drv;
		spi_driver_init_master(acc->spi_drv, spi_mode_0, msb_first);
	}

	return acc;
}

void bme280_access_destroy(bme280_access_p acc)
{
	LOG();
	if (acc) {
		free(acc);
	}
}

void bme280_access_read8(bme280_access_p acc, bme280_addr addr, uint8_t *out)
{
	LOG();
	spi_driver_start(acc->spi_drv);
	spi_driver_transmit(READ_ADDR(addr));
	spi_driver_transmit(READ_ADDR(addr));
	*out = spi_driver_read();
	spi_driver_stop(acc->spi_drv);
}

void bme280_access_read_n(bme280_access_p acc, bme280_addr addr, size_t n,
			  uint8_t *out)
{
	LOG();
	spi_driver_start(acc->spi_drv);
	spi_driver_transmit(READ_ADDR(addr));
	for (size_t i = 0; i < n; ++i) {
		spi_driver_transmit(READ_ADDR(addr));
		*out = spi_driver_read();
		out++;
	}
	spi_driver_stop(acc->spi_drv);
}

void bme280_access_read16(bme280_access_p acc, bme280_addr addr, uint16_t *out)
{
	LOG();
	(void)acc;
	(void)addr;
	(void)out;
}
void bme280_access_write(bme280_access_p acc, bme280_addr addr,
			 const uint8_t value)
{
	LOG();
	spi_driver_start(acc->spi_drv);
	spi_driver_transmit(WRITE_ADDR(addr));
	spi_driver_transmit(value);
	spi_driver_stop(acc->spi_drv);
}