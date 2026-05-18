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
    ESP_LOGCONFIG(TAG, "Setting up QMC5883L...");

    // Soft reset
    auto err = write_reg_(0x0D, 0x80);
    ESP_LOGD(TAG, "Soft reset: %d", err);
    delay(100);

    // Set/Reset Period
    err = write_reg_(0x0A, 0x01);
    ESP_LOGD(TAG, "Set/Reset Period reg 0x0A: %d", err);
    delay(50);

    // CR1: Continuous, +-8G, 50Hz, OSR=512
    err = write_reg_(0x0B, 0x1D);
    ESP_LOGD(TAG, "CR1 reg 0x0B: %d", err);
    delay(50);

    // CR2: no interrupts
    err = write_reg_(0x0C, 0x00);
    ESP_LOGD(TAG, "CR2 reg 0x0C: %d", err);
    delay(100);

    ESP_LOGCONFIG(TAG, "QMC5883L setup done");
  }

  void update() override {
    uint8_t reg = 0x00;
    auto err = this->write(&reg, 1);
    if (err != i2c::ERROR_OK) {
      ESP_LOGW(TAG, "Write failed: %d", err);
      return;
    }
    uint8_t data[6];
    err = this->read(data, 6);
    if (err != i2c::ERROR_OK) {
      ESP_LOGW(TAG, "Read failed: %d", err);
      return;
    }

    int16_t x = (int16_t)(data[0] | (data[1] << 8));
    int16_t y = (int16_t)(data[2] | (data[3] << 8));
    int16_t z = (int16_t)(data[4] | (data[5] << 8));

    ESP_LOGD(TAG, "Raw X=%d Y=%d Z=%d", x, y, z);

    if (x_sensor_) x_sensor_->publish_state(x * 0.03333f);
    if (y_sensor_) y_sensor_->publish_state(y * 0.03333f);
    if (z_sensor_) z_sensor_->publish_state(z * 0.03333f);
  }

  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  sensor::Sensor *x_sensor_{nullptr};
  sensor::Sensor *y_sensor_{nullptr};
  sensor::Sensor *z_sensor_{nullptr};

  i2c::ErrorCode write_reg_(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    return this->write(buf, 2);
  }
};

}  // namespace my_qmc5883l
}  // namespace esphome
