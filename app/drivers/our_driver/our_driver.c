#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT our_driver

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

struct our_driver_config {
    struct gpio_dt_spec led;
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

static DEVICE_API(sensor, api_l6task1) = {
    .sample_fetch = our_driver_sample_fetch,
    .channel_get = our_driver_channel_get,
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
                                                                             \
    DEVICE_DT_INST_DEFINE(inst,                                              \
                          our_driver_init,                                   \
                          NULL,                                              \
                          NULL,                                              \
                          &cfg_##inst,                         \
                          POST_KERNEL,                                       \
                          80,                       \
                          &api_l6task1);

DT_INST_FOREACH_STATUS_OKAY(OUR_DRIVER_DEFINE)
