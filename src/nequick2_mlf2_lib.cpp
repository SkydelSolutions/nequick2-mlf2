#include "nequick2_mlf2_lib.h"

#include <chrono>

#include "onnx_wrapper_interface.h"

extern "C"
{
#include "NeQuick2_MLF2.h"
}

struct NeQuick2MLF2Lib::Pimpl
{
  NeQuick2_MLF2_handle nequick2MLF2Handle;

  std::weak_ptr<OnnxWrapperInterface> hmF2Model;
  std::weak_ptr<OnnxWrapperInterface> foF2Model;

  Pimpl(std::weak_ptr<OnnxWrapperInterface>&& hmF2Model, std::weak_ptr<OnnxWrapperInterface>&& foF2Model) :
    nequick2MLF2Handle(NEQUICK_2_MLF2_INVALID_HANDLE),
    hmF2Model(std::move(hmF2Model)),
    foF2Model(std::move(foF2Model))
  {
    NeQuick2MLF2.init(&nequick2MLF2Handle);
  }

  ~Pimpl()
  {
    if (nequick2MLF2Handle != NEQUICK_2_MLF2_INVALID_HANDLE)
    {
      NeQuick2MLF2.close(nequick2MLF2Handle);
    }
  }

  bool isValid() const
  {
    return nequick2MLF2Handle != NEQUICK_2_MLF2_INVALID_HANDLE && !hmF2Model.expired() && !foF2Model.expired();
  }
};

NeQuick2MLF2Lib::NeQuick2MLF2Lib(std::weak_ptr<OnnxWrapperInterface>&& hmF2Model,
                                 std::weak_ptr<OnnxWrapperInterface>&& foF2Model) :
  m(std::make_unique<Pimpl>(std::move(hmF2Model), std::move(foF2Model)))
{
}

NeQuick2MLF2Lib::~NeQuick2MLF2Lib() = default;

bool NeQuick2MLF2Lib::isValid() const
{
  return m->isValid();
}

bool NeQuick2MLF2Lib::loadModipGridFromString(const std::string& data)
{
  if (!m->isValid())
  {
    return false;
  }

  if (NeQuick2MLF2.load_MODIP_grid(data.c_str()) != NEQUICK_OK)
  {
    return false;
  }

  return true;
}

double NeQuick2MLF2Lib::computeTecValue(const InputParameters& params) const
{
  if (!m->isValid())
  {
    return 0.0;
  }

  auto currentDate = std::chrono::year_month_day(std::chrono::year(params.year) / params.month / params.day);
  auto yearStart = std::chrono::year_month_day(std::chrono::year(params.year) / 1 / 1);
  uint16_t doy = (std::chrono::sys_days(currentDate) - std::chrono::sys_days(yearStart)).count() + 1;
  if (NeQuick2MLF2.set_time(m->nequick2MLF2Handle, params.month, params.timeUtcHours, doy) != NEQUICK_OK)
  {
    return 0.0;
  }

  if (NeQuick2MLF2.set_receiver_position(m->nequick2MLF2Handle,
                                         params.roverLongitudeDegrees,
                                         params.roverLatitudeDegrees,
                                         params.roverAltitudeMeters) != NEQUICK_OK)
  {
    return 0.0;
  }

  if (NeQuick2MLF2.set_satellite_position(m->nequick2MLF2Handle,
                                          params.satelliteLongitudeDegrees,
                                          params.satelliteLatitudeDegrees,
                                          params.satelliteAltitudeMeters) != NEQUICK_OK)
    return 0.0;

  if (NeQuick2MLF2.set_solar_activity_flux(m->nequick2MLF2Handle, params.f107) != NEQUICK_OK)
  {
    return 0.0;
  }

  auto hmF2Model = m->hmF2Model.lock();
  auto foF2Model = m->foF2Model.lock();
  if (!hmF2Model || !foF2Model)
  {
    return 0.0;
  }

  if (NeQuick2MLF2.set_F2_ML_layer_data(static_cast<void*>(hmF2Model.get()),
                                        static_cast<void*>(foF2Model.get()),
                                        params.year,
                                        params.month,
                                        params.day,
                                        params.timeUtcHours,
                                        params.ap,
                                        params.kp,
                                        params.smoothedF107_365) != NEQUICK_OK)
  {
    return 0.0;
  }

  double stec = 0;
  if (NeQuick2MLF2.get_total_electron_content(m->nequick2MLF2Handle, &stec) != NEQUICK_OK)
  {
    return 0.0;
  }

  return stec;
}
