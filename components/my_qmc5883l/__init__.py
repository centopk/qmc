import esphome.codegen as cg
import esphome.config_validation as cv

DEPENDENCIES = []

my_qmc5883l_ns = cg.esphome_ns.namespace("my_qmc5883l")

CONFIG_SCHEMA = cv.Schema({})

async def to_code(config):
    pass
