#include "device.h"

static struct device *devices[DEVICE_MAX];
static int device_count;

void device_init(void)
{
    device_count = 0;

    for (int i = 0; i < DEVICE_MAX; i++)
        devices[i] = 0;
}

int device_register(struct device *dev)
{
    if (device_count >= DEVICE_MAX)
        return -1;

    devices[device_count++] = dev;
    return 0;
}

struct device *device_get(const char *name)
{
    for (int i = 0; i < device_count; i++)
    {
        const char *a = devices[i]->name;
        const char *b = name;

        while (*a && *b && *a == *b)
        {
            a++;
            b++;
        }

        if (!*a && !*b)
            return devices[i];
    }

    return 0;
}
