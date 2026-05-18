import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    DEVICE_CLASS_MAGNETIC_FIELD,
    STATE_CLASS_MEASUREMENT,
    UNIT_MICROTESLA,
)

DEPENDENCIES = ["i2c", "sensor"]
AUTO_LOAD = ["sensor"]

my_qmc5883l_ns = cg.esphome_ns.namespace("my_qmc5883l")
MyQMC5883LComponent = my_qmc5883l_ns.class_(
    "MyQMC5883LComponent", cg.PollingComponent, i2c.I2CDevice
)

CONF_X = "field_strength_x"
CONF_Y = "field_strength_y"
CONF_Z = "field_strength_z"

_SENSOR_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_MICROTESLA,
    accuracy_decimals=1,
    device_class=DEVICE_CLASS_MAGNETIC_FIELD,
    state_class=STATE_CLASS_MEASUREMENT,
)

CONFIG_SCHEMA = (
    cv.Schema({
        cv.GenerateID(): cv.declare_id(MyQMC5883LComponent),
        cv.Optional(CONF_X, default={CONF_NAME: "Mag X"}): _SENSOR_SCHEMA,
        cv.Optional(CONF_Y, default={CONF_NAME: "Mag Y"}): _SENSOR_SCHEMA,
        cv.Optional(CONF_Z, default={CONF_NAME: "Mag Z"}): _SENSOR_SCHEMA,
    })
    .extend(cv.polling_component_schema("100ms"))
    .extend(i2c.i2c_device_schema(0x2C))  # адрес 0x2C по умолчанию
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    sens = await sensor.new_sensor(config[CONF_X])
    cg.add(var.set_x_sensor(sens))
    sens = await sensor.new_sensor(config[CONF_Y])
    cg.add(var.set_y_sensor(sens))
    sens = await sensor.new_sensor(config[CONF_Z])
    cg.add(var.set_z_sensor(sens))
