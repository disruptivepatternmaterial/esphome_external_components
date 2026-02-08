# Device status text sensor - list under text_sensor: in YAML
import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv

from .sensor import SEN5XComponent

DEPENDENCIES = ["text_sensor"]

CONF_SEN6X_ID = "sen6x_id"

CONFIG_SCHEMA = text_sensor.text_sensor_schema(
    icon="mdi:information-outline",
).extend(
    {
        cv.Required(CONF_SEN6X_ID): cv.use_id(SEN5XComponent),
    }
)


async def to_code(config):
    cg.add_define("SEN6X_USE_DEVICE_STATUS")
    parent = await cg.get_variable(config[CONF_SEN6X_ID])
    var = await text_sensor.new_text_sensor(config)
    cg.add(parent.set_device_status_text_sensor(var))
