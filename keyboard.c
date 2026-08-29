#include "device.h"

static int keyboard_init(void)
{
    return 0;
}

struct device keyboard_device = {
    "keyboard",
    1,
    keyboard_init
};
//same with this
