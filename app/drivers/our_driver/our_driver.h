#ifndef OUR_DRIVER_H
#define OUR_DRIVER_H

#include <zephyr/drivers/sensor.h>

typedef int (*our_driver_extension_func_t)(const struct device *dev, void *data);

__subsystem struct our_driver_api {
    struct sensor_driver_api base_api;
    our_driver_extension_func_t extension_func;
};

static inline int our_driver_extension_func(const struct device *dev, void *data)
{
    const struct our_driver_api *api = (const struct our_driver_api *)dev->api;

    return api->extension_func(dev, data);
}



#endif /* OUR_DRIVER_H */