#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <uartlib.h>
#include <pin.h>
#include <spi.h>

#define READ _BV(7)
#define CHIPVER 0xD0

int main(void)
{
    _delay_ms(200);

    uart_init();
    stdout = &uart_output;
    stdin = &uart_input;

    spi_driver spi_driver;
    digital_pin csPin = {.dir = &DDRB, .value = &PORTB, .mask = _BV(2)};
    spi_driver_init(&spi_driver, csPin);
    spi_driver_init_master(&spi_driver, spi_mode_0, msb_first);

    spi_driver_start(&spi_driver);
    spi_driver_transmit(CHIPVER | READ);
    spi_driver_transmit(CHIPVER | READ);
    char result = spi_driver_read();

    printf("result: 0x%02x", (int)result);

    while (1)
    {
        _delay_ms(500);
    }
    return 0;
}