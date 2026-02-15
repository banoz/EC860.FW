#include "EC860FW.h"

char msg_buffer[16];
int msg_index = 0;

int putchar(int c)
{
    while (ti_u0c1 == 0)
        ;
    u0tb = (unsigned char)c;
    return c;
}

void text_write(char *msg_string)
{
    char i = 0;
    /* This loop reads in the text string and puts it in the UART 0 transmit buffer */
    for (i = 0; msg_string[i]; i++)
    {
        while (ti_u0c1 == 0)
            ;
        u0tb = msg_string[i];
    }
}

void _uart0_tx_interrupt(void)
{
    if (txept_u0c0 == 1)
    {
        txept_u0c0 = 0;
        te_u0c1 = 0;

        u0tb = msg_buffer[msg_index++];
        if (msg_buffer[msg_index] == '\0')
        {
            msg_index = 0;
        }
    }
}

void _uart0_rx_interrupt(void)
{

    if (ri_u0c1 == 1)
    {
        ri_u0c1 = 0;
        re_u0c1 = 0;
        te_u0c1 = 1;
        // temp2 = u0rb;
    }
}