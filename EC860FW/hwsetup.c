#include "sfr_r827.h"
#include "hwsetup.h"

void port_config(void)
{
	/* Unblock PRC2 */
	prc2 = 1;

	pd0 = 0b11111000;

	/* Block PRC2 */
	prc2 = 0;

	pd1 = 0b11010100;

	pd3_1 = 1;
	pd3_3 = 0;
	pd3_4 = 1;
	pd3_5 = 0;
	pd3_6 = 1;
	pd3_7 = 0;

	pd4_5 = 0;
	pd5_3 = 1;
	pd5_4 = 1;
}

void clock_config(void)
{
	unsigned int i;
	/* ===== Clock Configuration ===== */
	/* Enable high-speed internal oscillator */
	prc0 = 1; /* Unblock clock control registers */
	cm14 = 0;
	fra2 = 0;
	fra00 = 1;
	for (i = 0; i <= 0xFF; i++)
		;
	fra01 = 1;
	ocd2 = 1;
	cm1 = cm1 & 0x3f;
	cm06 = 0;
	prc0 = 0; /* Lock clock control registers */
}

void adc_config()
{
	/* ===== ADC Configuration ===== */

	adic = 0x00;

	adcon0 = 0x00;

	adgsel0 = 1;
	ch0 = 0; // AN8, 1 for AN9
	ch1 = 0;
	ch2 = 1;
	md = 0; /* Single conversion mode */

	adcon1 = 0x00;
	bits = 1; /* 10-bit mode */
	vcut = 1; /* Connect Vref to AVcc */

	adcon2 = 0x00;
	smp = 0;

	/* ---- Enable A/D conversion interrupt ---- */
	adic = 0x01;
}

void interrupt_config(void)
{
	/* ===== INT0 Configuration ===== */

	int0pl = 1; /* Polarity: 1 = Both edges */
	int0ic = 0x00;/* Clear any pending INT0 flag */
	int0f0 = 1;
	int0f1 = 1;
	int0ic = 0x02; /* Enable INT0 interrupt with priority level 2 */
	int0en = 1;

	/* ===== INT3 Configuration ===== */

	int3pl = 1; /* Polarity: 1 = Both edges */
	int3ic = 0x00;
	int3f0 = 1;
	int3f1 = 1;
	int3ic = 0x02; /* Enable interrupt, priority level 2 */
	int3en = 1;
}

void timer_config(void)
{
	/* ===== Timer RB Configuration for 1ms ===== */

	/* Stop timer first */
	trbcr = 0x00;

	/* Timer RB mode register */
	trbmr = 0x00;
	/*
	 * Bit 7   : 0 = Timer mode
	 * Bit 6-4 : 000 = f1 (no division)
	 * Bit 3-0 : 0000 = Reserved
	 */

	/*
	 * Calculate prescaler and period for 1ms:
	 * f1 = 20MHz
	 * 1ms = 1000Hz
	 * 20MHz / 1000Hz = 20000 counts
	 *
	 * Using prescaler: 20000 = (TRBPRE + 1) * (TRBPR + 1)
	 * Example: (79 + 1) * (249 + 1) = 80 * 250 = 20000
	 */
	trbpre = 79; /* Prescaler: divide by 80 */
	trbpr = 249; /* Period: divide by 250 */

	/* Clear interrupt flag and set priority */
	trbic = 0x00;
	trbic = 0x01; /* Enable interrupt, priority level 1 */

	/* Start timer */
	trbcr = 0x01;

	/* Timer RE: free-running, 31.25µs per count */
	tstart_trecr1 = 0;
	while (tcstf_trecr1 != 0)
		; /* Wait for TRECR1 to clear */
	treic = 0;
	trerst_trecr1 = 1; /* Start Timer RE */
	trerst_trecr1 = 0; /* Clear start bit */
	trecsr = 0x65;	   // f8 = 20MHz / 8 (f8) / 2 / 16 (RCS2) = 78.125kHz, 12.8µs per count
	tremin = 4 - 1;	   // 4 counts = 51.2µs, 19.68125kHz interrupt rate
	toena_trecr1 = 0;  /* Disable TREO pin output */
	comie_trecr2 = 1;  /* Enable compare match interrupt */
	treic = 0x02;	   /* Enable interrupt, priority level 1 */
	tstart_trecr1 = 1; /* Start Timer RE */
	while (tcstf_trecr1 != 1)
		; /* Wait for TRECR1 to clear */
}

void serial_config(void)
{
	/* ===== Serial Communication Configuration ===== */
	u0mr = 0x05;
	s0tic = 1;
	s0ric = 1;
	te_u0c1 = 0;
	re_u0c1 = 0;
	u0mr = 0x05;
	u0c0 = 0b00001000;
	u0c1 = 0;
	u0brg = 0x15; //(unsigned char)(((f1_CLK_SPEED / 16) / BAUD_RATE) - 1);
	te_u0c1 = 1;
	re_u0c1 = 1;
}