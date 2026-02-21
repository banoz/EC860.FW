#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

typedef struct system_state
{
    /* receive */
	unsigned char pump;
	unsigned char valves;
	unsigned char target_temp_c;
	unsigned char target_temp_s;
	unsigned char leds;

    /* transmit */
	unsigned char millis;
    unsigned char temp_c;
	unsigned char temp_s;
	char pressure;
	unsigned char switches;
} system_state;

#endif // SYSTEM_STATE_H