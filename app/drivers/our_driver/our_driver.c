#include "our_driver.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT our_driver

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

struct our_driver_config {
    struct gpio_dt_spec led;
};

struct our_driver_data {
    int user_param;
};

static int our_driver_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
    const struct our_driver_config *cfg = dev->config;

    LOG_INF("Fetching sample for channel %d", chan);
    /* Turning LED on */
    gpio_pin_set_dt(&cfg->led, 1);
    return 0;
}

static int our_driver_channel_get(const struct device *dev, enum sensor_channel chan,
                               struct sensor_value *val)
{
    const struct our_driver_config *cfg = dev->config;

    LOG_INF("Getting channel value for channel %d", chan);
    /* Turning LED off */
    gpio_pin_set_dt(&cfg->led, 0);
    return 0;
}

static int our_driver_extension(const struct device *dev, void *data)
{
    struct our_driver_data *d = dev->data;
    int new_val = *(int *)data;
    LOG_INF("Executing extension function with data: %d", new_val);
    
    d->user_param = new_val;
    
    return 0;
}   

static struct our_driver_api api_l6task1 = {
    .base_api = {
        .sample_fetch = our_driver_sample_fetch,
        .channel_get = our_driver_channel_get,
    },
    .extension_func = our_driver_extension,
};

static int our_driver_init(const struct device *dev)
{
    const struct our_driver_config *cfg = dev->config;

    LOG_INF("Initializing our driver");
    
    if(!gpio_is_ready_dt(&cfg->led)) {
        LOG_ERR("GPIO device for LED is not ready");
        return -ENODEV;
    }
    return gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
}

#define OUR_DRIVER_DEFINE(inst) \
    static const struct our_driver_config cfg_##inst = {       \
        .led = GPIO_DT_SPEC_GET(DT_PHANDLE(DT_DRV_INST(inst), led), gpios),  \
    };                                                                       \
    static struct our_driver_data data_##inst = {                            \
        .user_param = 0, /* Default value */                                 \
    };                                                                       \
    DEVICE_DT_INST_DEFINE(inst,                                              \
                          our_driver_init,                                   \
                          NULL,                                              \
                          &data_##inst,                                      \
                          &cfg_##inst,                                       \
                          POST_KERNEL,                                       \
                          80,                                                \
                          &api_l6task1);

DT_INST_FOREACH_STATUS_OKAY(OUR_DRIVER_DEFINE)
