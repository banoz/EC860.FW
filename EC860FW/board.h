#ifndef BOARD_H
#define BOARD_H

#include "sfr_r827.h"
#include "system_state.h"

// PORT 0
#define PIN_S3_S6 p0_0        // IN  - US.INT.1
#define PIN_S2_S5 p0_1        // IN  - US.INT.2
#define PIN_S1_S4 p0_2        // IN  - US.INT.3
#define PIN_LED3_LED5 p0_3    // OUT - US.INT.4
#define PIN_LED2_LED4 p0_4    // OUT - US.INT.5
#define PIN_LED1_LED6 p0_5    // OUT - US.INT.6
#define PIN_LED123_ANODE p0_6 // OUT - US.INT.7
#define PIN_LED456_ANODE p0_7 // OUT - US.INT.8

// PORT 1
#define PIN_NTC_C p1_0    // AN8  - 100K
#define PIN_NTC_S p1_1    // AN9  - 100K
#define PIN_IC2 p1_2      // OUT
#define PIN_TXD0 p1_4     // UART TX
#define PIN_RXD0 p1_5     // UART RX
#define PIN_HEATER_C p1_6 // OUT
#define PIN_HEATER_S p1_7 // OUT

// PORT 3
#define PIN_EV1 p3_1      // OUT - GRAY
#define PIN_FLOW p3_3     // INT3
#define PIN_FLOW_PWR p3_4 // OUT - SW.2
#define PIN_SW1 p3_5      // IN
#define PIN_PUMP p3_6     // OUT
#define PIN_SW3 p3_7      // IN

// PORT 4
#define PIN_ZC p4_5 // INT0

// PORT 5
#define PIN_EV3 p5_3 // OUT - BROWN
#define PIN_EV2 p5_4 // OUT - WHITE

void board_tick(system_state *current_state);

unsigned long millis(void);

inline int clamp(int value, int min, int max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else
        return value;
}

#endif // BOARD_H