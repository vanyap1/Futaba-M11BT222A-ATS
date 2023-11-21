

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

enum port_pull_mode {
	PORT_PULL_OFF,
	PORT_PULL_UP,
};

enum port_dir {
	PORT_DIR_IN,
	PORT_DIR_OUT,
	PORT_DIR_OFF,
};


typedef struct gpio_setup
{
	uint8_t *port;
	uint8_t pin ;
} gpio;


void gpio_set_pin_level(gpio *GPIOx, const bool level);

void gpio_set_port_level(gpio *GPIOx, const uint8_t mask);

void gpio_set_pin_direction(gpio *GPIOx, const enum port_dir direction);

void gpio_set_pin_pull_mode(gpio *GPIOx, const enum port_pull_mode pull_mode);

void gpio_set_port_direction(gpio *GPIOx, const uint8_t mask);

static inline uint8_t gpio_get_port_level(gpio *GPIOx){
	uint8_t *tmp_port = GPIOx->port-2;
	return *tmp_port;
}



static inline bool gpio_get_pin_level(gpio *GPIOx)
{
	uint8_t *tmp_port = GPIOx->port-2;
	return *tmp_port & (1 << GPIOx->pin);
}

void gpio_toggle_pin_level(gpio *GPIOx);


