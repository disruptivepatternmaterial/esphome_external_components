# Sensor altitude number - list under number: in YAML
import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv

from .sensor import SEN5XComponent, sen6x_ns

DEPENDENCIES = ["number"]

Sen66AltitudeNumber = sen6x_ns.class_("Sen66AltitudeNumber", number.Number)

CONF_SEN6X_ID = "sen6x_id"
CONF_DEFAULT_VALUE = "default_value"
CONF_INITIAL_VALUE = "initial_value"

CONFIG_SCHEMA = number.number_schema(
    Sen66AltitudeNumber,
    unit_of_measurement="m",
).extend(
    {
        cv.Required(CONF_SEN6X_ID): cv.use_id(SEN5XComponent),
        cv.Optional(CONF_DEFAULT_VALUE): cv.int_range(0, 3000),
        cv.Optional(CONF_INITIAL_VALUE): cv.int_range(0, 3000),
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
    # default_value or initial_value: set altitude on sensor at setup
    initial = config.get(CONF_DEFAULT_VALUE, config.get(CONF_INITIAL_VALUE))
    if initial is not None:
        cg.add(var.set_initial_value(initial))
