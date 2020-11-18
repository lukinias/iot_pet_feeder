#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "struct-device.h"
#include "json/struct-json.h"
#include "json/decoder-i2t-json.h"
#include "config.h"
#include "devices.h"

float lastTotalAmount;
bool firstTime = true;
int steps_numbers;       // Value in degrees where is the motor
int degrees = 0;

/*** ACTION/PERFORMANCE FUNCTION *** This is the function where you will define the actions to be performed from the data retrieved from Tangle **/
void action(struct json *j)
{
	if (j->relative_timestamp < 300)	// If the delay is greater than 300 seconds (5 minutes) it is possible that the Streams channel is not receiving any more messages
	{	

		// Suppose that the value of total amount of cash is in the variable j->sensor[0].value[0]
		if (j->sensor[0].isEnable == true)
		{
			float totalAmount = atof(j->sensor[0].value[0]);
			int calibrate = atoi(j->sensor[0].value[1]);

			if ( calibrate == 1 )
			{
				// Calibrate step by step motor
				degrees += 5 * 1.4222222222;	// 5° turn
				while (degrees > steps_numbers)	// Turn in degrees
				{
					steps();
					steps_numbers = steps_numbers + 1;
				}
			}
			else
			{
				printf("\n\nTotal cash       -> ");
				printf("$%f\n\n", totalAmount);

				if (!firstTime && totalAmount > lastTotalAmount) // If the cash increase drop food and water
				{
					printf("Drop food and water.\n\n");

					// Drop food
					degrees += 60 * 1.4222222222;	// 60° turn
					while (degrees > steps_numbers)	// Turn in degrees
					{
						steps();
						steps_numbers = steps_numbers + 1;
					}
					turn_off();	// Turn off the step by step motor

					// Turn on de water pump for 5 seconds
					set_GPIO(23, 1);  // Put in HIGH GPIO23
					udelay_basics ( 5 * 1000000 );
					set_GPIO(23, 0);  // Put in LOW GPIO23
				}
				firstTime = false;
				lastTotalAmount = totalAmount;
			}
		}

		/********* Important Tools	*********************************************************************************/
		// 																											//
		//	int value_integer1 = atoi("43");			// atoi() function convert char* (string) to integer		//
		// 	int value_integer2 = atoi(j->sensor[2].value[0]);														//
		// 																											//
		//	float value_float1 = atof("43");			// atof() function convert char* (string) to float			//
		// 	float value_float2 = atof(j->sensor[2].value[1]);														//
		// 																											//
		//	bool isActive;																							//
		//	if (strcmp(j->sensor[2].value[1],"High" == 0)		// strcmp(char*,char*) function compare 2 strings	//
		//		isActive == true;																					//
		//																											//
		/************************************************************************************************************/

		// Send this command in shell to be able to simulate the examples:
		// curl --location --request POST 'YOUR_KEEPY_HOST:3002/messages' --header 'Content-Type: application/json' --data-raw '{"iot2tangle":[{"sensor":"Internal","data":[{"InternalTemperature":"47.24"}]},{"sensor":"Environmental","data":[{"Temperature":"50.0"},{"Humidity":"34.2"},{"Pressure":"998.20"}]},{"sensor":"Acoustic","data":[{"SoundLevel":"High"}]},{"sensor":"Light","data":[{"Light":"0"}]},{"sensor":"Accelerometer","data":[{"X":"3.98"},{"Y":"0.06"},{"Z":"9.20"}]},{"sensor":"Gyroscope","data":[{"X":"0.40"},{"Y":"-0.43"},{"Z":"-0.05"}]}],"device": "DEVICE_ID_1","timestamp": "1601653408"}'

	}
}
/***************************************************************/

void config(struct device *z)
{   
    /* User assignments */
    z->id = id_name;
        
    z->addr = address;
    z->addr_port = port;

    #ifdef MQTT
    	z->user_mqtt = user;
	    z->pass_mqtt = password;
	    z->top = topic;
    #endif

    #ifdef MICROCONTROLLER
        z->ssid_wifi = ssid_WiFi;
        z->pass_wifi = pass_WiFi;
    #endif

    z->isEnable_relay[0] = isEnable_Relay_1;
    z->isEnable_relay[1] = isEnable_Relay_2;
    z->isEnable_relay[2] = isEnable_Relay_3;
    z->isEnable_relay[3] = isEnable_Relay_4;

    z->interv = interval;

}

void initPeripherals(long* c) 
{
    *c = 0;		// Init counter
    
    #ifdef SHELLPRINT
    	welcome_msg();	// Printf in shell
    #endif
	
    init_LEDs();
    init_i2c();
    init_SPI();
    init_relay();
}


void led_blinks(int led, int iter, int usec)	// LED Blink function-> led: 0 Green LED, 1 Red LED - iter: iterations quantity - usec: delay time in usec
{	
    int i;
    for (i=0;i<iter;i++)
    {
	led_GPIO(led, 1);
	udelay_basics (usec);
	led_GPIO(led, 0);
	udelay_basics (usec);
    }
}

void connectNetwork(struct device *z, bool first_t)
{	
	#ifdef MICROCONTROLLER			
	if (first_t)
	{				
		while ( !connectAttempt(z->ssid_wifi, z->pass_wifi) )    /* Attempt to connect to the network via WiFi, in RaspberryPi only check connection to the network. */
		{
			led_blinks(0, 1, 600000);	// Blink in green GREEN - ERROR 0 (No WiFi connection);
			led_blinks(1, 1, 600000);	// Blink in green RED - ERROR 0 (No WiFi connection);
		}
	}
	#endif
	if ( !init_socket(z->addr, z->addr_port, z->user_mqtt, z->pass_mqtt, first_t) )     /* Check Endpoint */
	{	
		udelay_basics ( 100000 );
		led_blinks(1, 3, 70000);	// Blink in green RED - ERROR 1 (Bad connection with the endpoint);
	}
}

bool get_data_tangle(char* js, struct device *z, long* c)
{
	++(*c);
	d_collect_msg(c);
	
	bool b_socket = get_json(js, z->addr, z->addr_port, z->top, z->user_mqtt, z->pass_mqtt, z->interv);
    if (b_socket)
    {
		led_blinks(0, 2, 60000);	// Blink in green LED;
    	print_json(js);
    }
    else
		led_blinks(1, 3, 70000);	// Blink in green RED - ERROR 1 (Bad connection with the endpoint);

	
    return b_socket;
}


void decode_json(char* js, struct json *j)
{
   	printf ("Decoding I2T Json...");
	if ( recover_json(js, j) )
	{
		printf ("		Decoding completed successfully\n\nAvailable Variables in Device '%s' (according to last Json received):\n", j->id);
		for (int i=0; i < MAX_SENSORS; i++)
		{	
			if (j->sensor[i].isEnable)
			{
				printf ("    - Sensor[%d] -> %s    -- ", i, j->sensor[i].id);
				for (int k=0; k < j->sensor[i].num_values; k++)
				{
					printf ("'%s.%s' - ", j->sensor[i].id, j->sensor[i].name[k]);
				}
				printf ("\n");
			}
		}
		j->relative_timestamp = 12;
		printf("        -- Data published in Tangle %ld seconds ago -- (Harcoded value, this feature is not available yet)\n", j->relative_timestamp);
	}
	else
		printf ("\nThe Json is not I2T format, please use the -Json Iot2Tangle format- to decode\n");
}

void clear_data(struct json *j)
{
	for (int i = 0; i < MAX_SENSORS; i++) 
	{
		j->sensor[i].isEnable = false;
		sprintf(j->sensor[i].id, " ");
		j->sensor[i].num_values = 0;
		for (int k = 0; k < MAX_VALUE; k++) 
		{
			sprintf(j->sensor[i].name[k], " ");
			sprintf(j->sensor[i].value[k], " ");
		}
	}		
}

void t_delay(long d, long l) 
{  
	if (l >= d)		/* To prevent crashes */
		l = d;
    udelay_basics ( (d - l) * 1000000 );	/* Time set by user  minus  loss time by operation */ 
}


long take_time() 
{  
    return take_time_basics();
}
