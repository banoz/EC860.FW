#ifndef HWSETUP_H
#define HWSETUP_H

#define f1_CLK_SPEED 20000000
#define BAUD_RATE 57600

void port_config(void);
void clock_config(void);
void interrupt_config(void);
void adc_config(void);
void timer_config(void);
void serial_config(void);

#endif // HWSETUP_H