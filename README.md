# esphome_external_components

collection of esphome components

## SEN66
Just because i didn't wanted to wait for the official component after getting my SEN66 evaluation kit!

Only works with SEN66.

### Differences SEN5X to SEN6X
* Each type SEN65,SEN66,... has a dedicated READ command
* getProductName is not working (on my device at least)
* Temp/Hum accelaration has changed from 3 modes to fine params (not implemented)
* Auto clean has been removed, guess should be triggered from outside now

### Read-failure handling (I2C wedge recovery)

On the ESP-IDF new I2C master driver, the bus can get stuck so that every read
fails (`clear bus failed` / `reset hardware failed`, `last_error_ = 2`). A soft
reset cannot recover this because the reset command travels over the same dead
bus. Previously `update()` simply logged the error and returned **without
publishing**, so the entities held their last value forever — a frozen reading
that looks live and silently feeds any downstream averages.

The component now counts consecutive failed reads (`MAX_CONSECUTIVE_READ_FAILURES`,
default 3). Once the threshold is crossed it publishes `NaN` to every sensor
(`publish_all_nan_()`), and the counter resets on the next successful read.
Publishing `NaN`:

* marks the entities `unavailable` instead of leaving a stale value, and
* lets a `NaN`-based reboot watchdog in the device YAML fire — a full reboot
  re-inits the I2C peripheral and clears the wedged bus.

Recommended device-side watchdog (reboots after ~5 min uptime if the SEN66
readings are `NaN`):

```yaml
interval:
  - interval: 1min
    then:
      - if:
          condition:
            lambda: |-
              if (id(device_uptime).state < 300) return false;
              return isnan(id(sen66_temp).state) ||
                     isnan(id(sen66_humidity).state) ||
                     isnan(id(PM_2_5).state);
          then:
            - logger.log: "Sensor failure detected - rebooting device"
            - delay: 5s
            - lambda: 'App.safe_reboot();'
```
