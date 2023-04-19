#ifndef PIN_H
#define PIN_H

#include <stdint.h>

struct digital_pin
{
    volatile uint8_t *dir;
    volatile uint8_t *value;
    uint8_t mask;
};
typedef struct digital_pin digital_pin;

void set_out(digital_pin *pin);
void set_in(digital_pin *pin);

void set_high(digital_pin *pin);
void set_low(digital_pin *pin);

int is_high(digital_pin *pin);
int is_low(digital_pin *pin);

#endif