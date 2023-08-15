#include <avr/interrupt.h>
#include <avr/io.h>
#include <bme280.h>
#include <bme280_access.h>
#include <i2c_scan.h>
#include <pin.h>
#include <spi.h>
#include <string.h>
#include <uart.h>
#include <util/delay.h>

FILE uart_output;
FILE uart_input;

#define USE_SPI 0

void routine()
{
	bme280_access *bme_access = NULL;

	if (USE_SPI) {
		static spi_driver spi_driver;
		static digital_pin csPin = {
		    .dir = &DDRB, .value = &PORTB, .mask = _BV(2)};
		spi_driver_init(&spi_driver, csPin);
		bme_access = bme280_access_init_spi(&spi_driver);

	} else {

		//i2c_scan();
		static i2c_driver i2c_driver;
		i2c_driver_init(&i2c_driver, 0x76);
		bme_access = bme280_access_init_i2c(&i2c_driver);
	}

	bme280_p bme = bme280_init(bme_access);

	if (!bme) {
		printf("failed to alloc bme280_p");
	} else {
		if (bme280_reset(bme)) {
			printf("%s: failed to reset bme280\n", __func__);
			return;
		}

		_delay_ms(10);
		int available = bme280_is_available(bme);
		printf("%s: Bme is %s!\n", __func__,
		       available ? "available"
				 : "unavailable, check cables or SPI config");
		if (available) {
			bme280_load_control_registers(bme);
			bme280_load_coefficients(bme);

			bme280_set_standby(bme, sb_20ms);

			bme280_set_filter(bme, filter_4);

			bme280_osr_settings osr_settings = {
			    .hum = osr_skip, .press = osr_4, .temp = osr_1};
			bme280_set_osr_settings(bme, osr_settings);

			_delay_ms(10);
			bme280_control_registers controlRegisters;
			bme280_get_control_registers(bme, &controlRegisters);
			bme280_print_control_registers(&controlRegisters,
						       stdout);

			bme280_set_mode(bme, mode_normal);

			_delay_ms(10);
			bme280_get_control_registers(bme, &controlRegisters);
			bme280_print_control_registers(&controlRegisters,
						       stdout);

			bme280_reads reads;
			while (1) {

				if (bme280_read(bme, &reads)) {
					printf("%s: bme280_read failed\n",
					       __func__);
				}

				printf("T: %ld, H: %lu, P: %lu\n",
				       reads.temperature, reads.humidity,
				       reads.pressure);
				_delay_ms(500);
			}
		}
	}
}

int main(void)
{
	_delay_ms(200);

	uart_init();
	uart_output = open_uart_ouput();
	uart_input  = open_uart_ouput();
	stdout	    = &uart_output;
	stdin	    = &uart_input;

	routine();

	while (1) {
		_delay_ms(500);
	}
	return 0;
}