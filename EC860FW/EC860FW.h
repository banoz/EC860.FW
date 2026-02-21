#ifndef EC860FW_H
#define EC860FW_H

#include "sfr_r827.h"

#define ENABLE_IRQ       \
    {                    \
        _asm(" FSET I"); \
    }

#define DISABLE_IRQ      \
    {                    \
        _asm(" FCLR I"); \
    }

#endif // EC860FW_H