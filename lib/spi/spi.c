#include "spi.h"
#include <stdio.h>

#define LOG() printf("%s\n", __func__)

void spi_driver_init(spi_driver *spi_driver, digital_pin cs_pin)
{
    LOG();
    digital_pin mosi_pin = {.dir = &DDRB, .value = &PORTB, .mask = _BV(3)};
    digital_pin miso_pin = {.dir = &DDRB, .value = &PORTB, .mask = _BV(4)};
    digital_pin sck_pin = {.dir = &DDRB, .value = &PORTB, .mask = _BV(5)};

    spi_driver->cs_pin = cs_pin;
    spi_driver->miso_pin = miso_pin;
    spi_driver->mosi_pin = mosi_pin;
    spi_driver->sck_pin = sck_pin;
}

static void spi_driver_set_mode(spi_mode mode)
{
    LOG();
    switch (mode)
    {
    case spi_mode_0:
        SPCR &= ~(1 << CPOL);
        SPCR &= ~(1 << CPHA);
        break;
    case spi_mode_1:
        SPCR &= ~(1 << CPOL);
        SPCR |= (1 << CPHA);
        break;
    case spi_mode_2:
        SPCR |= (1 << CPOL);
        SPCR &= ~(1 << CPHA);
        break;
    default:
        SPCR |= (1 << CPOL);
        SPCR |= (1 << CPHA);
    }
}

static void spi_driver_set_bit_order(bit_order bitOrder)
{
    LOG();
    switch (bitOrder)
    {
    case msb_first:
        SPCR &= ~(1 << DORD);
        break;
    case lsb_first:
    default:
        SPCR |= (1 << DORD);
    }
}

void spi_driver_init_master(spi_driver *drv, spi_mode spiMode, bit_order bitOrder)
{
    LOG();
    /* Set MOSI and SCK output, all others input */
    set_out(&drv->mosi_pin);
    set_out(&drv->sck_pin);
    set_in(&drv->miso_pin);

    /* Set CS out*/
    set_out(&drv->cs_pin);

    /* Set all to low*/
    set_low(&drv->mosi_pin);
    set_low(&drv->miso_pin);
    set_low(&drv->sck_pin);

    /* Set CS to high*/
    set_high(&drv->cs_pin);

    /* Enable SPI, Master, set clock rate fck/16 */
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

    spi_driver_set_mode(spiMode);
    spi_driver_set_bit_order(bitOrder);
}

void spi_driver_transmit(char cData)
{
    LOG();
    /* Start transmission */
    SPDR = cData;
    /* Wait for transmission complete */
    while (!(SPSR & (1 << SPIF)))
    {
    }
}

char spi_driver_read()
{
    char c = SPDR;
    return c;
}

void spi_driver_start(spi_driver *drv)
{
    set_low(&drv->cs_pin);
}
void spi_driver_stop(spi_driver *drv)
{
    set_high(&drv->cs_pin);
}