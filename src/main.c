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
		bme280_reset(bme);
		_delay_ms(10);
		int available = bme280_check_availability(bme);
		printf("Bme is %s!\n",
		       available ? "available"
				 : "unavailable, check cables or SPI config");
		if (available) {
			bme280_load_control_registers(bme);
			bme280_load_coefficients(bme);
			bme280_set_standby(bme, sb_20ms);
			bme280_set_filter(bme, filter_8);
			bme280_set_mode(bme, mode_normal);

			bme280_osr_settings osr_settings = {
			    .hum = osr_4, .press = osr_4, .temp = osr_4};
			bme280_set_osr_settings(bme, osr_settings);
			
			_delay_ms(10);
			bme280_load_control_registers(bme);
			
			bme280_set_mode(bme, mode_force);
			bme280_set_mode(bme, mode_normal);

			_delay_ms(10);
			bme280_load_control_registers(bme);

			bme280_reads reads;
			while (1) {
				bme280_read(bme, &reads);
				printf("T: %d, H: %d, P: %d\n",
				       reads.temperature, reads.humidity,
				       reads.pressure);
				_delay_ms(2000);
			}
		}
	}

	while (1) {
		_delay_ms(500);
	}
	return 0;
}