#ifndef UART_COMMS_H
#define UART_COMMS_H

#include "system_state.h"

int uart_init(void);

void uart_tick(system_state *current_state);

#endif // UART_COMMS_H