#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "../drivers/our_driver/our_driver.h"

/* The devicetree node identifier for the "led0" alias. */
#define LED_NODE DT_ALIAS(app_led)

static const struct device * driver = DEVICE_DT_GET(DT_NODELABEL(our_driver0));

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    bool led_state = true;

    if (!device_is_ready(driver)) {
        LOG_ERR("Our driver is not ready");
        return -ENODEV;
    }

    int ret = sensor_sample_fetch(driver);
    if (ret < 0) {
        LOG_ERR("Failed to fetch sample from our driver");
        return -EIO;
    }

    int value = 42;
    our_driver_extension_func(driver, &value);

    while (1) {
        
        led_state = !led_state;
        if(led_state) {
            ret = sensor_sample_fetch(driver);
            if (ret < 0) {
                LOG_ERR("Failed to fetch sample from our driver");
                return -EIO;
            }
        } else {
            ret = sensor_channel_get(driver, SENSOR_CHAN_ALL , NULL);
            if (ret < 0) {
                LOG_ERR("Failed to get channel value from our driver");
                return -EIO;
            }
        }
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
    return 0;
}
