#include <successdisplay.h>
#include <display.h>

void successdisplay_page(void)
{
    display_write("Thank you!", 4, 4);
    display_flush();
}