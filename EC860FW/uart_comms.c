#include "EC860FW.h"
#include "board.h"
#include "ring_buffer.h"
#include "uart_comms.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

char msg_buffer[16];
int msg_index = 0;

static rbd_t _rbd;
static unsigned char _rbmem[32] = {0};

static rbd_t _tbd;
static unsigned char _tbmem[32] = {0};

unsigned long uart_tx_tick = 0;
static unsigned char rx_buffer[6] = {0};
volatile int rx_buffer_idx = 0;

int uart_init(void)
{
    rb_attr_t attr_r = {_rbmem, ARRAY_SIZE(_rbmem)};
    rb_attr_t attr_t = {_tbmem, ARRAY_SIZE(_tbmem)};

    ring_buffer_init(&_rbd, &attr_r);
    ring_buffer_init(&_tbd, &attr_t);

    return 0;
}

void uart_tick(system_state *current_state)
{
    unsigned char c;

    unsigned long currentMillis = millis();

    unsigned char echo = 0;

    while (ring_buffer_get(_rbd, &c) == 0)
    {
        if (c == (unsigned char)0xAA) // Start of packet. Sender must avoid sending 0xAA in payload
        {
            rx_buffer_idx = 0;
            continue;
        }
        else if (rx_buffer_idx >= sizeof(rx_buffer))
        {
            continue;
        }

        if (rx_buffer_idx < sizeof(rx_buffer))
        {
            rx_buffer[rx_buffer_idx++] = c;
        }

        if (rx_buffer_idx >= sizeof(rx_buffer))
        {
            // Validate checksum
            unsigned char checksum = rx_buffer[0] ^ rx_buffer[1] ^ rx_buffer[2] ^ rx_buffer[3] ^ rx_buffer[4];
            if (checksum == rx_buffer[5])
            {
                // Valid packet - update state
                current_state->pump = rx_buffer[0];
                current_state->valves = rx_buffer[1];
                current_state->target_temp_c = rx_buffer[2];
                current_state->target_temp_s = rx_buffer[3];
                current_state->leds = rx_buffer[4];

                echo = 1;
            }
        }
    }

    if (echo > 0)
    {
        unsigned char checksum = current_state->millis ^ current_state->temp_c ^ current_state->temp_s ^ current_state->pressure ^ current_state->switches;

        ring_buffer_put(_tbd, 0xAA);
        ring_buffer_put(_tbd, current_state->millis);
        ring_buffer_put(_tbd, current_state->temp_c);
        ring_buffer_put(_tbd, current_state->temp_s);
        ring_buffer_put(_tbd, current_state->pressure);
        ring_buffer_put(_tbd, current_state->switches);
        ring_buffer_put(_tbd, checksum);
    }

    /* Start transmission if TX buffer has data and transmitter is ready */
    if (ti_u0c1 == 1)
    {
        if (ring_buffer_get(_tbd, &c) == 0)
        {
            u0tb = c;
        }
    }
}

int putchar(int c)
{
    while (ti_u0c1 == 0)
        ;
    u0tb = (unsigned char)c;
    return c;
}

void _uart0_tx_interrupt(void)
{
    unsigned char c;

    if (ti_u0c1 == 1)
    {
        if (ring_buffer_get(_tbd, &c) == 0)
        {
            u0tb = c;
        }
    }
}

void _uart0_rx_interrupt(void)
{
    unsigned char c;

    c = u0rb;

    ring_buffer_put(_rbd, c);
}