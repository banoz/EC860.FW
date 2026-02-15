#include "sfr_r827.h"
#include "hwsetup.h"
#include "board.h"
#include "EC860FW.h"

void main(void);

void main(void)
{
	unsigned long lastMillis = 0;
	unsigned long currentMillis = 0;

	/* Disable watchdog timer */
	wdts = 0x00;

	DISABLE_IRQ

	/* Hardware setup */
	port_config();

	PIN_HEATER_C = 0;
	PIN_HEATER_S = 0;

	PIN_LED1_LED6 = 0;
	PIN_LED2_LED4 = 0;
	PIN_LED3_LED5 = 0;

	PIN_LED123_ANODE = 1;
	PIN_LED456_ANODE = 1;

	PIN_FLOW_PWR = 1;

	clock_config();
	interrupt_config();
	adc_config();
	timer_config();
	serial_config();

	/* Enable global interrupts */
	ENABLE_IRQ
	
	PIN_FLOW_PWR = 0;

	while (1)
	{
		currentMillis = millis();

		if (currentMillis > lastMillis)
		{
			lastMillis = currentMillis;

			board_tick();
		}
	}
}
