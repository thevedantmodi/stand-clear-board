#include "ee14lib.h"
#include <display.h>
#include <stdio.h>
#include <stdlib.h>
#include <systick.h>

int main(void)
{
    host_serial_init(9600);
    SysTick_initialize();
    display_init();

    while (1) {
        /* code */
    }
}