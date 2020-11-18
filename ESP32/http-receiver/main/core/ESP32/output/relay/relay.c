#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "relay.h"
#include "../../basics.h"

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

#define NUM_RELAYS 4
#define RELAY_1 19		// RELAY 1 in GPIO19
#define RELAY_2 18		// RELAY 2 in GPIO18
#define RELAY_3 5		// RELAY 3 in GPIO5
#define RELAY_4 17		// RELAY 4 in GPIO17

const int num[NUM_RELAYS] = { RELAY_1, RELAY_2, RELAY_3,  RELAY_4};

void init_relay()
{
	for (int i=0; i<NUM_RELAYS; i++)
	{
		gpio_pad_select_gpio(num[i]);
		gpio_set_direction(num[i], GPIO_MODE_OUTPUT);
		gpio_set_level(num[i], 0);	// Init as LOW
	}
}

void set_relay_GPIO(int relay, int mode)
{
	gpio_set_level(num[relay], mode);
}

// Steps
void steps()
{
	gpio_set_level(RELAY_4, 1);
	gpio_set_level(RELAY_3, 1);
	gpio_set_level(RELAY_2, 0);
	gpio_set_level(RELAY_1, 0);

	udelay_basics ( 5 * 2500 );

	gpio_set_level(RELAY_4, 0);
	gpio_set_level(RELAY_3, 1);
	gpio_set_level(RELAY_2, 1);
	gpio_set_level(RELAY_1, 0);

	udelay_basics ( 5 * 2500 );

	gpio_set_level(RELAY_4, 0);
	gpio_set_level(RELAY_3, 0);
	gpio_set_level(RELAY_2, 1);
	gpio_set_level(RELAY_1, 1);

	udelay_basics ( 5 * 2500 );

	gpio_set_level(RELAY_4, 1);
	gpio_set_level(RELAY_3, 0);
	gpio_set_level(RELAY_2, 0);
	gpio_set_level(RELAY_1, 1);

	udelay_basics ( 5 * 2500 );
}

// Turn off the step by step motor
void turn_off()
{
	gpio_set_level(RELAY_4, 0);
	gpio_set_level(RELAY_3, 0);
	gpio_set_level(RELAY_2, 0);
	gpio_set_level(RELAY_1, 0);
}
