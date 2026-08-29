#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

#define DEVICE_MAX 16

struct device {
    const char *name;
    uint8_t type;

    int (*init)(void);
    int (*read)(void *buffer, uint32_t size);
    int (*write)(const void *buffer, uint32_t size);
};

void device_init(void);
int device_register(struct device *dev);
struct device *device_get(const char *name);

#endif
