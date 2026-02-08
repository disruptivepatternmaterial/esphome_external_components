# CO2 automatic self-calibration switch - list under switch: in YAML
import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from .sensor import SEN5XComponent, sen6x_ns

DEPENDENCIES = ["switch"]

Sen66ASCSwitch = sen6x_ns.class_("Sen66ASCSwitch", switch.Switch)

CONF_SEN6X_ID = "sen6x_id"

CONFIG_SCHEMA = switch.switch_schema(Sen66ASCSwitch, icon="mdi:molecule-co2").extend(
    {
        cv.Required(CONF_SEN6X_ID): cv.use_id(SEN5XComponent),
    }
)


async def to_code(config):
    cg.add_define("SEN6X_USE_ASC_SWITCH")
    parent = await cg.get_variable(config[CONF_SEN6X_ID])
    var = await switch.new_switch(config)
    cg.add(var.set_parent(parent))
    cg.add(parent.set_asc_switch(var))
