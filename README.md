# EC860

Renesas R8C/27

R5F21274NFP

|PORT  |CONN       |DESC                                  |SW         |                             |
|------|-----------|--------------------------------------|-----------|-----------------------------|
| P0_0 | US.INT.1  |                                      | IN        |  S3, S6                     |
| P0_1 | US.INT.2  |                                      | IN        |  S2, S5                     |
| P0_2 | US.INT.3  |                                      | IN        |  S1, S4                     |
| P0_3 | US.INT.4  |                                      | OUT       |  LED3, LED5                 |
| P0_4 | US.INT.5  |                                      | OUT       |  LED2, LED4                 |
| P0_5 | US.INT.6  |                                      | OUT       |  LED1, LED6                 |
| P0_6 | US.INT.7  |                                      | OUT       |  LED[1,2,3] ANODE, S[4,5,6] |
| P0_7 | US.INT.8  |                                      | OUT       |  LED[4,5,6] ANODE, S[1,2,3] |
| P1_0 | NTC_C     | 100K                                 | AN8       |                             |
| P1_1 | NTC_S     | 100K                                 | AN9       |                             |
| P1_2 | IC2       |                                      | OUT       |                             |
| P1_3 | P_UP      | 10K pullup                           |           |                             |
| P1_4 | TXD0      | UART TX                              |           |                             |
| P1_5 | RXD0      | UART RX                              |           |                             |
| P1_6 | HEATER_C  |                                      | OUT       |                             |
| P1_7 | HEATER_S  |                                      | OUT       |                             |
| P3_1 | EV1       |                                      | OUT       | GRAY                        |
| P3_3 | FLOW      |                                      | INT3      |                             |
| P3_4 | SW.2      | FLOW_PWR                             | OUT       |                             |
| P3_5 | SW.1      |                                      | IN        |                             |
| P3_6 | PUMP      |                                      | OUT       |                             |
| P3_7 | SW.3      |                                      | IN        |                             |
| P4_5 | ZC        |                                      | INT0      |                             |
| P4_6 | P_UP      | 10K pullup                           |           |                             |
| P4_7 | P_UP      | 10K pullup                           |           |                             |
| P5_3 | EV3       |                                      | OUT       | BROWN                       |
| P5_4 | EV2       |                                      | OUT       | WHITE                       |

|   |PROG  |
|---|------|
| 1 | GND  |
| 2 | MODE |
| 3 | NRST |
| 4 | VCC  |
