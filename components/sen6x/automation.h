#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "sen6x.h"

namespace esphome {
namespace sen6x {

template<typename... Ts> class StartFanAction : public Action<Ts...> {
 public:
  explicit StartFanAction(SEN5XComponent *sen6x) : sen6x_(sen6x) {}

  void play(Ts... x) override { this->sen6x_->start_fan_cleaning(); }

 protected:
  SEN5XComponent *sen6x_;
};

template<typename... Ts> class PerformCo2RecalibrationAction : public Action<Ts...> {
 public:
  explicit PerformCo2RecalibrationAction(SEN5XComponent *sen6x) : sen6x_(sen6x) {}

  TEMPLATABLE_VALUE(float, target_ppm)

  void play(Ts... x) override {
    uint16_t ppm = 400;
    if (this->target_ppm_.has_value()) {
      ppm = static_cast<uint16_t>(this->target_ppm_.value(x...));
    }
    this->sen6x_->perform_forced_co2_recalibration(ppm);
  }

 protected:
  SEN5XComponent *sen6x_;
};

}  // namespace sen6x
}  // namespace esphome
