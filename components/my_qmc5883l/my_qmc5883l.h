#pragma once
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace my_qmc5883l {

static const char *const TAG = "my_qmc5883l";

class MyQMC5883LComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_x_sensor(sensor::Sensor *s) { x_sensor_ = s; }
  void set_y_sensor(sensor::Sensor *s) { y_sensor_ = s; }
  void set_z_sensor(sensor::Sensor *s) { z_sensor_ = s; }

  void setup() override {
    ESP_LOGE(TAG, "SETUP CALLED addr=0x%02X", this->address_);
    write_reg_(0x0D, 0x80);
    delay(100);
    write_reg_(0x0A, 0x01);
    delay(50);
    write_reg_(0x0B, 0x1D);
    delay(50);
    write_reg_(0x0C, 0x00);
    delay(100);
    ESP_LOGE(TAG, "SETUP DONE");
  }

  void update() override {
    ESP_LOGE(TAG, "UPDATE CALLED");
    uint8_t reg = 0x00;
    if (this->write(&reg, 1) != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "Write failed");
      return;
    }
    uint8_t data[6];
    if (this->read(data, 6) != i2c::ERROR_OK) {
      ESP_LOGE(TAG, "Read failed");
      return;
    }
    int16_t x = (int16_t)(data[0] | (data[1] << 8));
    int16_t y = (int16_t)(data[2] | (data[3] << 8));
    int16_t z = (int16_t)(data[4] | (data[5] << 8));
    ESP_LOGE(TAG, "X=%d Y=%d Z=%d", x, y, z);
    if (x_sensor_) x_sensor_->publish_state(x * 0.03333f);
    if (y_sensor_) y_sensor_->publish_state(y * 0.03333f);
    if (z_sensor_) z_sensor_->publish_state(z * 0.03333f);
  }

  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  sensor::Sensor *x_sensor_{nullptr};
  sensor::Sensor *y_sensor_{nullptr};
  sensor::Sensor *z_sensor_{nullptr};

  void write_reg_(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    this->write(buf, 2);
  }
};

}  // namespace my_qmc5883l
}  // namespace esphome
