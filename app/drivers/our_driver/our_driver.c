#include "our_driver.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include <zephyr/shell/shell.h>

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

    /* Turning LED on */
    gpio_pin_set_dt(&cfg->led, 1);
    return 0;
}

static int our_driver_channel_get(const struct device *dev, enum sensor_channel chan,
                               struct sensor_value *val)
{
    const struct our_driver_config *cfg = dev->config;

    /* Turning LED off */
    gpio_pin_set_dt(&cfg->led, 0);

    if (val) {
        val->val1 = 123; /* Dummy value */
        val->val2 = 456; /* Dummy value */
    }
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


static int cmd_our_driver_fetch(const struct shell *sh, size_t argc, char **argv)
{
    our_driver_sample_fetch(DEVICE_DT_GET(DT_NODELABEL(our_driver0)), SENSOR_CHAN_ALL);
    return 0;
}

static int cmd_our_driver_get(const struct shell *sh, size_t argc, char **argv)
{
    struct sensor_value val;
    our_driver_channel_get(DEVICE_DT_GET(DT_NODELABEL(our_driver0)), SENSOR_CHAN_ALL, &val);
    shell_print(sh, "Sensor value fetched (dummy): val1=%d, val2=%d", val.val1, val.val2);
    return 0;
}

static int cmd_our_driver_info(const struct shell *sh, size_t argc, char **argv)
{
    /* print device name and ready state */
    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
    shell_print(sh, "Device name: %s", dev->name);
    shell_print(sh, "Device is %s", device_is_ready(dev) ? "ready" : "not ready");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_our_driver,
    SHELL_CMD(fetch, NULL, "Fetch sample from our driver", cmd_our_driver_fetch),
    SHELL_CMD(read, NULL, "Get channel value from our driver", cmd_our_driver_get),
    SHELL_CMD(info, NULL, "Prints driver info", cmd_our_driver_info),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(our_driver, &sub_our_driver, "Commands for our driver", NULL);