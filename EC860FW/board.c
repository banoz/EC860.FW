#include "EC860FW.h"
#include "board.h"
#include "uart_comms.h"
#include "stdio.h"

typedef struct psm_state
{
	unsigned int psm_a;		  // PSM accumulator
	unsigned char psm_range;  // PSM range: 0-127 (7-bit control)
	unsigned char psm_value;  // Current PSM power setting
	unsigned int psm_counter; // Count of pump activation cycles
} psm_state;

typedef struct led_state
{
	unsigned char led1 : 1; // LED 1 state
	unsigned char led2 : 1; // LED 2 state
	unsigned char led3 : 1; // LED 3 state
	unsigned char led4 : 1; // LED 4 state
	unsigned char led5 : 1; // LED 5 state
	unsigned char led6 : 1; // LED 6 state
} led_state;

typedef struct switch_state
{
	unsigned char s1 : 1; // Switch 1 state
	unsigned char s2 : 1; // Switch 2 state
	unsigned char s3 : 1; // Switch 3 state
	unsigned char s4 : 1; // Switch 4 state
	unsigned char s5 : 1; // Switch 5 state
	unsigned char s6 : 1; // Switch 6 state
} switch_state;

typedef struct ntc_mapping
{
	unsigned int adc_value; // ADC reading from NTC
	unsigned char temp_c;	// Corresponding temperature in Celsius
} ntc_mapping;

volatile psm_state pump_psm = {0, 0x7F, 0, 0}; // Pump PSM (0-127)
// psm_state coffee_boiler_psm = {0, 0x7F, 0, 0}; // Coffee boiler PSM (0-127)

switch_state switches = {0, 0, 0, 0, 0, 0}; // Switch states
led_state leds = {0, 0, 0, 0, 0, 0};		// LED states

const ntc_mapping temp_mapping[] = {
	{90U, 25U},
	{110U, 30U},
	{160U, 40U},
	{230U, 50U},
	{300U, 60U},
	{390U, 70U},
	{470U, 80U},
	{560U, 90U},
	{640U, 100U},
	{710U, 110U},
	{790U, 120U},
	{870U, 130U},
	{950U, 140U},
	{1030U, 150U},
	{1110U, 160U}};

volatile unsigned long system_time_ms = 0;
volatile unsigned char zero_crossed = 0;
volatile unsigned int ntc_c_adc_value = 0;
volatile unsigned int ntc_s_adc_value = 0;
volatile int pressure = 0;

volatile unsigned char pressure_pwm_counter = 0;
volatile unsigned char pressure_pwm_rise_counter = 0;
volatile unsigned char pressure_duty_cycle = 0;

void check_zc(void);
void set_leds_switches(unsigned long);
unsigned int temp_interpolation(unsigned int);
void adc_poll(void);
char calculateSkip(psm_state *);

unsigned long uart_counter = 0;

void test_stuff(unsigned long currentMillis)
{
	char msg_buffer[32];

	if (uart_counter <= currentMillis)
	{
		leds.led5 = !leds.led5; // Toggle LED5 every 500ms

		while (uart_counter <= currentMillis)
		{
			uart_counter += 500;
		}

		// if (leds.led5)
		// 	sprintf(msg_buffer, "S: %u\n", ntc_s_adc_value);
		// else
		// 	sprintf(msg_buffer, "C: %u\n", ntc_c_adc_value);

		// sprintf(msg_buffer, "[%i]\n", (int)pressure);
		sprintf(msg_buffer, "%u;%u\n", ntc_c_adc_value, temp_interpolation(ntc_c_adc_value));

		text_write(msg_buffer);
	}

	adc_poll();

	if (switches.s5 == 1)
	{
		pump_psm.psm_value = 10;
	}
	else if (switches.s4 == 1)
	{
		pump_psm.psm_value = 50;
	}
	else
	{
		pump_psm.psm_value = 0;
	}

	leds.led6 = switches.s1;
	leds.led4 = switches.s2;
	// leds.led5 = switches.s3;

	leds.led1 = switches.s4;
	leds.led2 = switches.s5;
	leds.led3 = switches.s6;
}

void board_tick()
{
	unsigned long currentMillis = millis();

	set_leds_switches(currentMillis);

	check_zc();

	pressure = ((pressure_duty_cycle - 10) * 12 /* pressure range for transducer */) / 8;

	test_stuff(currentMillis); // TODO
}

void set_leds_switches(unsigned long currentMillis)
{
	if (currentMillis & 0x01)
	{
		switches.s4 = PIN_S1_S4 == 1 ? 1 : 0;
		switches.s5 = PIN_S2_S5 == 1 ? 1 : 0;
		switches.s6 = PIN_S3_S6 == 1 ? 1 : 0;

		PIN_LED1_LED6 = leds.led6 == 0 ? 1 : 0;
		PIN_LED2_LED4 = leds.led4 == 0 ? 1 : 0;
		PIN_LED3_LED5 = leds.led5 == 0 ? 1 : 0;

		PIN_LED123_ANODE = 1;
		PIN_LED456_ANODE = 0;
	}
	else
	{
		switches.s1 = PIN_S1_S4 == 1 ? 1 : 0;
		switches.s2 = PIN_S2_S5 == 1 ? 1 : 0;
		switches.s3 = PIN_S3_S6 == 1 ? 1 : 0;

		PIN_LED1_LED6 = leds.led1 == 0 ? 1 : 0;
		PIN_LED2_LED4 = leds.led2 == 0 ? 1 : 0;
		PIN_LED3_LED5 = leds.led3 == 0 ? 1 : 0;

		PIN_LED123_ANODE = 0;
		PIN_LED456_ANODE = 1;
	}
}

unsigned long millis()
{
	return system_time_ms;
}

unsigned int temp_interpolation(unsigned int adc_value) // 10 * centigrade
{
	unsigned char i = sizeof(temp_mapping) / sizeof(ntc_mapping) - 1;
	ntc_mapping previous = temp_mapping[i];

	if (adc_value >= previous.adc_value)
	{
		return previous.temp_c * 10;
	}

	i--;

	for (; i > 0; i--)
	{
		ntc_mapping current = temp_mapping[i];

		if (adc_value >= current.adc_value)
		{
			// Linear interpolation
			unsigned int temp_c = current.temp_c * 10 +
								  ((adc_value - current.adc_value) * (previous.temp_c - current.temp_c) * 10) /
									  (previous.adc_value - current.adc_value);

			return temp_c;
		}

		previous = current;
	}

	return 0; // Out of range
}

void adc_poll()
{
	if (adst == 0)
	{
		ch0 = !ch0; // Toggle between AN8 and AN9 each poll

		// Start ADC conversion
		adst = 1;
	}
}

/// PSM (Pulse Skip Modulation) algorithm
// Implements Bresenham-like algorithm to determine if pump should run this cycle
// Accumulates psm_value each cycle; when it exceeds psm_range, pump activates

/**
 * @brief Calculate whether to skip the current pump cycle
 * @note Called on each AC zero-crossing to implement PSM control
 */
char calculateSkip(psm_state *psm)
{
	char psm_skip = 0;

	psm->psm_a += psm->psm_value;

	if (psm->psm_a >= psm->psm_range)
	{
		psm->psm_a -= psm->psm_range;
		psm_skip = 0;
	}
	else
	{
		psm_skip = 1;
	}

	if (psm->psm_a > psm->psm_range)
	{
		psm->psm_a = 0;
		psm_skip = 0;
	}

	if (!psm_skip)
	{
		psm->psm_counter++;
	}

	return psm_skip;
}

/**
 * @brief Check and handle AC zero-crossing events
 * @note Called from main loop to process zero_crossed flag set by INT0 ISR
 * @note Triggers ADC polling and PSM calculation on each zero-crossing
 */
void check_zc()
{
	if (zero_crossed > 0)
	{
		adc_poll();

		PIN_PUMP = calculateSkip(&pump_psm) == 0 ? 1 : 0;
		// PIN_HEATER_C = calculateSkip(&coffee_boiler_psm) == 0 ? 1 : 0;

		zero_crossed = 0;
	}
}

void _ad_converter_interrupt(void);

void _ad_converter_interrupt()
{
	if (ch0 == 0) // AN8
	{
		ntc_c_adc_value = ad; // Read AN8 value
	}
	else // AN9
	{
		ntc_s_adc_value = ad; // Read AN9 value
	}
}

void _int0_interrupt(void);

void _int0_interrupt()
{
	zero_crossed = 1;
}

void _int3_interrupt(void);

void _int3_interrupt()
{
	if (PIN_FLOW == 1)
	{
		tstart_trecr1 = 0;
		while (tcstf_trecr1 != 0)
			; /* Wait for TRECR1 to clear */

		if (pressure_pwm_counter == 0)
		{
			pressure_duty_cycle = 0;
		}
		else
		{
			pressure_duty_cycle = (unsigned char)(((unsigned int)(100U * pressure_pwm_rise_counter)) / pressure_pwm_counter);
		}

		pressure_pwm_rise_counter = 0;
		pressure_pwm_counter = 0;

		tstart_trecr1 = 1; /* Start Timer RE */
		while (tcstf_trecr1 != 1)
			; /* Wait for TRECR1 to clear */
	}
	else
	{
		/* Falling edge */
		pressure_pwm_rise_counter = pressure_pwm_counter;
	}
}

void _timer_rb_interrupt(void);

void _timer_rb_interrupt()
{
	system_time_ms++;
}

void _timer_re_interrupt(void);

void _timer_re_interrupt()
{
	pressure_pwm_counter++;
}