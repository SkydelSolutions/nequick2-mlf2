#pragma once

#include <memory>
#include <string>

class OnnxWrapperInterface;

class NeQuick2MLF2Lib
{
public:
  struct InputParameters
  {
    double roverLatitudeDegrees = 0;
    double roverLongitudeDegrees = 0;
    double roverAltitudeMeters = 0;

    double satelliteLatitudeDegrees = 0;
    double satelliteLongitudeDegrees = 0;
    double satelliteAltitudeMeters = 0;

    double timeUtcHours = 0;
    uint8_t month = 0;

    double f107 = 0;
    double ap = 0;
    double kp = 0;
    double smoothedF107_365 = 0;

    uint16_t year = 0;
    uint8_t day = 0;
  };

  NeQuick2MLF2Lib(std::weak_ptr<OnnxWrapperInterface>&& hmF2Model, std::weak_ptr<OnnxWrapperInterface>&& foF2Model);
  ~NeQuick2MLF2Lib();

  bool isValid() const;

  bool loadModipGridFromString(const std::string& data);

  double computeTecValue(const InputParameters& params) const;

private:
  struct Pimpl;
  std::unique_ptr<Pimpl> m;
};
