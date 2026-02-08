# Sensor altitude number - list under number: in YAML
import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv

from .sensor import SEN5XComponent, sen6x_ns

DEPENDENCIES = ["number"]

Sen66AltitudeNumber = sen6x_ns.class_("Sen66AltitudeNumber", number.Number)

CONF_SEN6X_ID = "sen6x_id"

CONFIG_SCHEMA = number.number_schema(
    Sen66AltitudeNumber,
    unit_of_measurement="m",
).extend(
    {
        cv.Required(CONF_SEN6X_ID): cv.use_id(SEN5XComponent),
    }
)


async def to_code(config):
    cg.add_define("SEN6X_USE_ALTITUDE")
    parent = await cg.get_variable(config[CONF_SEN6X_ID])
    var = await number.new_number(
        config,
        min_value=0,
        max_value=3000,
        step=1,
    )
    cg.add(var.set_parent(parent))
    cg.add(parent.set_altitude_number(var))
