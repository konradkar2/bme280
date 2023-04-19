#include "pin.h"
#include <util/delay.h>

#define DELAY() _delay_us(1)
void set_out(digital_pin *pin)
{
    *pin->dir |= pin->mask;
}

void set_in(digital_pin *pin)
{
    *pin->dir &= ~pin->mask;
}

void set_high(digital_pin *pin)
{
    *pin->value |= pin->mask;
}

void set_low(digital_pin *pin)
{
    *pin->value &= ~pin->mask;
}

int is_high(digital_pin *pin)
{
    return *pin->value & pin->mask;
}

int is_low(digital_pin *pin)
{
    return !is_high(pin);
}