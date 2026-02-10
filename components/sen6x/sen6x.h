#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/sensirion_common/i2c_sensirion.h"
#include "esphome/core/application.h"
#include "esphome/core/preferences.h"
#ifdef SEN6X_USE_DEVICE_STATUS
#include "esphome/components/text_sensor/text_sensor.h"
#endif
#ifdef SEN6X_USE_ASC_SWITCH
#include "esphome/components/switch/switch.h"
#endif
#ifdef SEN6X_USE_ALTITUDE
#include "esphome/components/number/number.h"
#endif

namespace esphome {
namespace sen6x {

enum ERRORCODE {
  COMMUNICATION_FAILED,
  SERIAL_NUMBER_IDENTIFICATION_FAILED,
  MEASUREMENT_INIT_FAILED,
  PRODUCT_NAME_FAILED,
  FIRMWARE_FAILED,
  UNKNOWN
};

// Shortest time interval of 3H for storing baseline values.
// Prevents wear of the flash because of too many write operations
const uint32_t SHORTEST_BASELINE_STORE_INTERVAL = 10800;
// Store anyway if the baseline difference exceeds the max storage diff value
const uint32_t MAXIMUM_STORAGE_DIFF = 50;

struct Sen5xBaselines {
  int32_t state0;
  int32_t state1;
} PACKED;  // NOLINT


struct GasTuning {
  uint16_t index_offset;
  uint16_t learning_time_offset_hours;
  uint16_t learning_time_gain_hours;
  uint16_t gating_max_duration_minutes;
  uint16_t std_initial;
  uint16_t gain_factor;
};

struct TemperatureCompensation {
  int16_t offset;
  int16_t normalized_offset_slope;
  uint16_t time_constant;
  uint16_t slot;  // SEN66: 0..4, default 0
};

class SEN5XComponent : public PollingComponent, public sensirion_common::SensirionI2CDevice {
 public:
  float get_setup_priority() const override { return setup_priority::DATA; }
  void setup() override;
  void dump_config() override;
  void update() override;

  enum Sen5xType { SEN50, SEN54, SEN55, UNKNOWN };

  void set_pm_1_0_sensor(sensor::Sensor *pm_1_0) { pm_1_0_sensor_ = pm_1_0; }
  void set_pm_2_5_sensor(sensor::Sensor *pm_2_5) { pm_2_5_sensor_ = pm_2_5; }
  void set_pm_4_0_sensor(sensor::Sensor *pm_4_0) { pm_4_0_sensor_ = pm_4_0; }
  void set_pm_10_0_sensor(sensor::Sensor *pm_10_0) { pm_10_0_sensor_ = pm_10_0; }
  void set_pm_0_10_sensor(sensor::Sensor *pm_0_10) { pm_0_10_sensor_ = pm_0_10; }

  void set_voc_sensor(sensor::Sensor *voc_sensor) { voc_sensor_ = voc_sensor; }
  void set_nox_sensor(sensor::Sensor *nox_sensor) { nox_sensor_ = nox_sensor; }
  void set_humidity_sensor(sensor::Sensor *humidity_sensor) { humidity_sensor_ = humidity_sensor; }
  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_co2_sensor(sensor::Sensor *co2) { co2_sensor_ = co2; }
  void set_store_baseline(bool store_baseline) { store_baseline_ = store_baseline; }
  void set_voc_algorithm_tuning(uint16_t index_offset, uint16_t learning_time_offset_hours,
                                uint16_t learning_time_gain_hours, uint16_t gating_max_duration_minutes,
                                uint16_t std_initial, uint16_t gain_factor) {
    GasTuning tuning_params;
    tuning_params.index_offset = index_offset;
    tuning_params.learning_time_offset_hours = learning_time_offset_hours;
    tuning_params.learning_time_gain_hours = learning_time_gain_hours;
    tuning_params.gating_max_duration_minutes = gating_max_duration_minutes;
    tuning_params.std_initial = std_initial;
    tuning_params.gain_factor = gain_factor;
    voc_tuning_params_ = tuning_params;
  }
  void set_nox_algorithm_tuning(uint16_t index_offset, uint16_t learning_time_offset_hours,
                                uint16_t learning_time_gain_hours, uint16_t gating_max_duration_minutes,
                                uint16_t gain_factor) {
    GasTuning tuning_params;
    tuning_params.index_offset = index_offset;
    tuning_params.learning_time_offset_hours = learning_time_offset_hours;
    tuning_params.learning_time_gain_hours = learning_time_gain_hours;
    tuning_params.gating_max_duration_minutes = gating_max_duration_minutes;
    tuning_params.std_initial = 50;
    tuning_params.gain_factor = gain_factor;
    nox_tuning_params_ = tuning_params;
  }
  void set_temperature_compensation(float offset, float normalized_offset_slope, uint16_t time_constant,
                                    uint16_t slot = 0) {
    TemperatureCompensation temp_comp;
    temp_comp.offset = offset * 200;
    temp_comp.normalized_offset_slope = normalized_offset_slope * 10000;
    temp_comp.time_constant = time_constant;
    temp_comp.slot = (slot > 4) ? 0 : slot;
    temperature_compensation_ = temp_comp;
  }
  void perform_forced_co2_recalibration(uint16_t target_ppm);
  bool start_fan_cleaning();
  /** Soft-reset sensor (0xD304) and restart measurements after 1.2 s. Use when readings are stuck or wrong. */
  void reset_sensor();

#ifdef SEN6X_USE_DEVICE_STATUS
  void set_device_status_text_sensor(text_sensor::TextSensor *t) { device_status_text_sensor_ = t; }
#endif
#ifdef SEN6X_USE_ASC_SWITCH
  void set_co2_asc(bool enable);
  void set_asc_switch(switch_::Switch *s) { asc_switch_ = s; }
#endif
#ifdef SEN6X_USE_ALTITUDE
  void set_altitude(uint16_t altitude_m);
  void set_altitude_number(number::Number *n) { altitude_number_ = n; }
#endif

 protected:
  bool write_tuning_parameters_(uint16_t i2c_command, const GasTuning &tuning);
  bool write_temperature_compensation_(const TemperatureCompensation &compensation);
#ifdef SEN6X_USE_DEVICE_STATUS
  std::string format_device_status_(uint32_t status) const;
#endif
#ifdef SEN6X_USE_ASC_SWITCH
  bool read_co2_asc_(bool &enabled);
#endif
#ifdef SEN6X_USE_ALTITUDE
  bool read_altitude_(uint16_t &altitude_m);
#endif
  ERRORCODE error_code_;
  bool initialized_{false};
  sensor::Sensor *pm_1_0_sensor_{nullptr};
  sensor::Sensor *pm_2_5_sensor_{nullptr};
  sensor::Sensor *pm_4_0_sensor_{nullptr};
  sensor::Sensor *pm_10_0_sensor_{nullptr};
  sensor::Sensor *pm_0_10_sensor_{nullptr};
  // SEN54 and SEN55 only
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};
  sensor::Sensor *voc_sensor_{nullptr};
  // SEN55 only
  sensor::Sensor *nox_sensor_{nullptr};
  sensor::Sensor *co2_sensor_{nullptr};
#ifdef SEN6X_USE_DEVICE_STATUS
  text_sensor::TextSensor *device_status_text_sensor_{nullptr};
#endif
#ifdef SEN6X_USE_ASC_SWITCH
  switch_::Switch *asc_switch_{nullptr};
#endif
#ifdef SEN6X_USE_ALTITUDE
  number::Number *altitude_number_{nullptr};
#endif

  std::string product_name_;
  uint8_t serial_number_[4];
  uint16_t firmware_version_;
  Sen5xBaselines voc_baselines_storage_;
  bool store_baseline_;
  uint32_t seconds_since_last_store_;
  ESPPreferenceObject pref_;
  optional<GasTuning> voc_tuning_params_;
  optional<GasTuning> nox_tuning_params_;
  optional<TemperatureCompensation> temperature_compensation_;
};

#ifdef SEN6X_USE_ASC_SWITCH
class Sen66ASCSwitch : public switch_::Switch {
 public:
  void set_parent(SEN5XComponent *parent) { parent_ = parent; }
  void write_state(bool state) override;

 protected:
  SEN5XComponent *parent_{nullptr};
};
#endif

#ifdef SEN6X_USE_ALTITUDE
class Sen66AltitudeNumber : public number::Number {
 public:
  void set_parent(SEN5XComponent *parent) { parent_ = parent; }
  void set_initial_value(uint16_t v) { initial_value_ = static_cast<int>(v); }
  int get_initial_value() const { return initial_value_; }
  void control(float value) override;

 protected:
  SEN5XComponent *parent_{nullptr};
  int initial_value_{-1};
};
#endif

}  // namespace sen6x
}  // namespace esphome
