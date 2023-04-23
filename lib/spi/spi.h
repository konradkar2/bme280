#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <pin.h>

enum spi_mode
{
    spi_mode_0,
    spi_mode_1,
    spi_mode_2,
    spi_mode_3
};
typedef enum spi_mode spi_mode;

enum bit_order
{
    msb_first,
    lsb_first
};
typedef enum bit_order bit_order;

struct spi_driver
{
    digital_pin sck_pin;
    digital_pin mosi_pin;
    digital_pin miso_pin;
    digital_pin cs_pin;
};
typedef struct spi_driver spi_driver;

void spi_driver_init(spi_driver *drv, digital_pin cs_pin);
void spi_driver_init_master(spi_driver *drv, spi_mode spiMode, bit_order bitOrder);
void spi_driver_start(spi_driver *drv);
void spi_driver_stop(spi_driver *drv);
void spi_driver_transmit(uint8_t cData);
uint8_t spi_driver_read();

#endif