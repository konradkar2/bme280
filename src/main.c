#include <avr/interrupt.h>
#include <avr/io.h>
#include <bme280.h>
#include <bme280_access.h>
#include <pin.h>
#include <spi.h>
#include <string.h>
#include <uartlib.h>
#include <util/delay.h>

int main(void)
{
	_delay_ms(200);

	uart_init();
	stdout = &uart_output;
	stdin = &uart_input;

	spi_driver spi_driver;
	digital_pin csPin = {.dir = &DDRB, .value = &PORTB, .mask = _BV(2)};
	spi_driver_init(&spi_driver, csPin);

	bme280_p bme = bme280_init(&spi_driver);
	if (!bme) {
		printf("failed to alloc bme280_p");
	} else {
		int available = bme280_check_availability(bme);
		printf("Bme is %s!\n",
		       available
			   ? "available"
			   : "unavailable, check cables or SPI config");
		int reset_succ = bme280_reset(bme);
		printf("Reset %s!", reset_succ ? "succesfull" : "failed");
	}

	while (1) {
		_delay_ms(500);
	}
	return 0;
}