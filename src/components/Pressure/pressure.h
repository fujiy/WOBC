#include <library/wobc.h>
#include <Wire.h>
#include "src/BME280I2C.h"

namespace component {

class Pressure: public process::Component {
public:
  static const uint8_t component_id = 0x25; // TBD
  static const uint8_t telemetry_id = 'E'; // TBD

  Pressure(TwoWire& wire, unsigned sample_freq_hz = 1);

protected:
  TwoWire& wire_;
  BME280I2C bme;

  void setup() override;

  class SampleTimer: public process::Timer {
  public:
    SampleTimer(Pressure& pressure_ref, BME280I2C& bme_ref, unsigned interval_ms);

  protected:
    void callback() override;

  private:
    BME280I2C& bme_;
    Pressure& pressure_;
  } sample_timer_;
};

}
