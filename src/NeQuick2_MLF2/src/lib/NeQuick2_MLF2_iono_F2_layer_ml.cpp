/** NeQuick2-MLF2 F2 layer Machine Learning part
 *
 * @author Safran Trusted 4D SAS Nantes
 * @ingroup NeQuick2-MLF2
 * @copyright Safran Trusted 4D SAS, 2026<br>
 *  This software has been released as free and open source software
 *  under the terms of the European Union Public Licence (EUPL), version 1.2<br>
 *  Questions? Submit your query at https://safran-navigation-timing.com/support-hub/skydel/
 * @file
 */
extern "C"
{
#include "NeQuick2_MLF2_iono_F2_layer_ml.h"

#include "NeQuickG_JRC_error.h"
}

#include <array>
#include <chrono>
#include <cmath>

#include "onnx_wrapper_interface.h"

using namespace OnnxUtils;

namespace
{
constexpr auto ML_MODEL_NULL = 12;
constexpr auto ML_RUN_ERROR = 13;

constexpr size_t HM_F2_MODEL_INPUT_PARAMETERS_COUNT = 19;
constexpr size_t FO_F2_MODEL_INPUT_PARAMETERS_COUNT = 24;

constexpr auto DAYS_IN_YEAR_COUNT = 365;

struct MLInputParameters
{
  MLInputParameters(uint16_t _year,
                    uint8_t _month,
                    uint8_t _day,
                    double_t ut,
                    double_t _ap,
                    double_t _kp,
                    double_t _smoothed_f107_365)
  {
    updateStaticParameters(_year, _month, _day, ut, _ap, _kp, _smoothed_f107_365);
  }

  void updateStaticParameters(uint16_t _year,
                              uint8_t _month,
                              uint8_t _day,
                              double_t ut,
                              double_t _ap,
                              double_t _kp,
                              double_t _smoothed_f107_365)
  {
    year = _year;
    month = _month;
    day = _day;
    ut_hour = ut;
    ap = _ap;
    kp = _kp;
    smoothed_f107_365 = _smoothed_f107_365;
    month_sin = std::sin(NEQUICK_G_JRC_TWO_PI * month / NEQUICK_2_MLF2_MONTHS_COUNT);
    month_cos = std::cos(NEQUICK_G_JRC_TWO_PI * month / NEQUICK_2_MLF2_MONTHS_COUNT);
    auto currentDate = std::chrono::year_month_day(std::chrono::year(year) / month / day);
    auto yearStart = std::chrono::year_month_day(std::chrono::year(year) / 1 / 1);
    doy = (std::chrono::sys_days(currentDate) - std::chrono::sys_days(yearStart)).count() + 1;
    doy_sin = std::sin(NEQUICK_G_JRC_TWO_PI * doy / (DAYS_IN_YEAR_COUNT + (currentDate.year().is_leap() ? 1 : 0)));
    doy_cos = std::cos(NEQUICK_G_JRC_TWO_PI * doy / (DAYS_IN_YEAR_COUNT + (currentDate.year().is_leap() ? 1 : 0)));
    ut_hour_sin = std::sin(NEQUICK_G_JRC_TWO_PI * ut_hour / NEQUICK_2_MLF2_TIME_DAY_IN_HOURS);
    ut_hour_cos = std::cos(NEQUICK_G_JRC_TWO_PI * ut_hour / NEQUICK_2_MLF2_TIME_DAY_IN_HOURS);
  }

  void update(const NeQuick2_MLF2_time_t* const pTime,
              const NeQuick2_MLF2_modip_context_t* const pModip,
              const NeQuick2_MLF2_solar_activity_t* const pSolar_activity,
              const position_t* const pCurrent_position,
              angle_restricted_t solar_declination)
  {
    longitude_hmF2 = pCurrent_position->longitude.degree > NEQUICK_G_JRC_CIRCLE_DEGREES / 2.
                       ? pCurrent_position->longitude.degree - NEQUICK_G_JRC_CIRCLE_DEGREES
                       : pCurrent_position->longitude.degree;
    lt_hour = NeQuick2_MLF2_time_get_local(pTime, longitude_hmF2);
    lt_hour_sin = std::sin(NEQUICK_G_JRC_TWO_PI * lt_hour / NEQUICK_2_MLF2_TIME_DAY_IN_HOURS);
    lt_hour_cos = std::cos(NEQUICK_G_JRC_TWO_PI * lt_hour / NEQUICK_2_MLF2_TIME_DAY_IN_HOURS);
    latitude_hmF2 = pCurrent_position->latitude.degree;
    lon_hmF2_sin = pCurrent_position->longitude.sin;
    lon_hmF2_cos = pCurrent_position->longitude.cos;
    modip_hmF2 = pModip->modip_degree;
    f107 = pSolar_activity->solar_radio_flux_sfu;
    sdelta = solar_declination.sin;
    cdelta = solar_declination.cos;
  }

  DataType year = 0.;
  DataType month = 0.;
  DataType month_sin = 0.;
  DataType month_cos = 0.;
  DataType day = 0.;
  DataType doy = 0.;
  DataType doy_sin = 0.;
  DataType doy_cos = 0.;
  DataType ut_hour = 0.;
  DataType ut_hour_sin = 0.;
  DataType ut_hour_cos = 0.;
  DataType lt_hour = 0.;
  DataType lt_hour_sin = 0.;
  DataType lt_hour_cos = 0.;
  DataType latitude_hmF2 = 0.;
  DataType longitude_hmF2 = 0.;
  DataType lon_hmF2_sin = 0.;
  DataType lon_hmF2_cos = 0.;
  DataType modip_hmF2 = 0.;
  DataType f107 = 0.;
  DataType ap = 0.;
  DataType kp = 0.;
  DataType sdelta = 0.;
  DataType cdelta = 0.;
  DataType smoothed_f107_365 = 0.;
};

struct InputParametersHmF2 final : InputParameters
{
  InputParametersHmF2(const MLInputParameters& allParameters) :
    parameters({allParameters.month_sin,
                allParameters.doy_sin,
                allParameters.doy_cos,
                allParameters.ut_hour_sin,
                allParameters.ut_hour_cos,
                allParameters.lt_hour,
                allParameters.lt_hour_sin,
                allParameters.lt_hour_cos,
                allParameters.latitude_hmF2,
                allParameters.longitude_hmF2,
                allParameters.lon_hmF2_cos,
                allParameters.lon_hmF2_sin,
                allParameters.modip_hmF2,
                allParameters.f107,
                allParameters.kp,
                allParameters.ap,
                allParameters.sdelta,
                allParameters.cdelta,
                allParameters.smoothed_f107_365})
  {
  }

  DataType* data() override { return parameters.data(); }
  size_t size() const override { return parameters.size(); }

  std::array<DataType, HM_F2_MODEL_INPUT_PARAMETERS_COUNT> parameters;
};

struct InputParametersFoF2 final : InputParameters
{
  InputParametersFoF2(const MLInputParameters& allParameters) :
    parameters({allParameters.month,
                allParameters.month_sin,
                allParameters.month_cos,
                allParameters.day,
                allParameters.doy,
                allParameters.doy_sin,
                allParameters.doy_cos,
                allParameters.ut_hour,
                allParameters.ut_hour_sin,
                allParameters.ut_hour_cos,
                allParameters.lt_hour,
                allParameters.lt_hour_sin,
                allParameters.lt_hour_cos,
                allParameters.latitude_hmF2,
                allParameters.longitude_hmF2,
                allParameters.lon_hmF2_sin,
                allParameters.lon_hmF2_cos,
                allParameters.modip_hmF2,
                allParameters.f107,
                allParameters.ap,
                allParameters.kp,
                allParameters.sdelta,
                allParameters.cdelta,
                allParameters.smoothed_f107_365})

  {
  }

  DataType* data() override { return parameters.data(); }
  size_t size() const override { return parameters.size(); }

  std::array<DataType, FO_F2_MODEL_INPUT_PARAMETERS_COUNT> parameters;
};

int32_t runModel(OnnxWrapperInterface* const model, InputParameters&& inputParameters, DataType& output)
{
  try
  {
    output = model->run(std::move(inputParameters)).front();
  }
  catch (const std::exception& e)
  {
    NEQUICK_ERROR_RETURN(NEQUICK_ERROR_SRC_F2_LAYER, ML_RUN_ERROR, e.what());
  }
  catch (...)
  {
    NEQUICK_ERROR_RETURN(NEQUICK_ERROR_SRC_F2_LAYER, ML_RUN_ERROR, "Unknow error during ML model run");
  }

  return NEQUICK_OK;
}

} // namespace

struct F2_ml_context
{
  OnnxWrapperInterface* hm_model = nullptr;
  OnnxWrapperInterface* f0_model = nullptr;

  MLInputParameters* input_parameters = nullptr;
};

static F2_ml_context context = {};

int32_t NeQuick2_MLF2_set_F2_ml_layer_data(void* hm_model,
                                           void* f0_model,
                                           uint16_t year,
                                           uint8_t month,
                                           uint8_t day,
                                           double_t ut,
                                           double_t ap,
                                           double_t kp,
                                           double_t smoothed_f107_365)
{
  if (!hm_model)
  {
    NEQUICK_ERROR_RETURN(NEQUICK_ERROR_SRC_F2_LAYER, ML_MODEL_NULL, "The Hm F2 ML model is null");
  }
  context.hm_model = reinterpret_cast<OnnxWrapperInterface*>(hm_model);

  if (!f0_model)
  {
    NEQUICK_ERROR_RETURN(NEQUICK_ERROR_SRC_F2_LAYER, ML_MODEL_NULL, "The Fo F2 ML model is null");
  }
  context.f0_model = reinterpret_cast<OnnxWrapperInterface*>(f0_model);

  if (!context.input_parameters)
  {
    context.input_parameters = new MLInputParameters(year, month, day, ut, ap, kp, smoothed_f107_365);
    if (!context.input_parameters)
    {
      NEQUICK_ERROR_RETURN(NEQUICK_ERROR_SRC_MEMORY,
                           NEQUICK_ERROR_CODE_NO_RESOURCES,
                           "Not enough resources to allocate ML input parameters on the heap");
    }
  }
  else
  {
    context.input_parameters->updateStaticParameters(year, month, day, ut, ap, kp, smoothed_f107_365);
  }

  return NEQUICK_OK;
}

void NeQuick2_MLF2_F2_ml_layer_close()
{
  if (context.input_parameters)
  {
    delete context.input_parameters;
  }

  context.hm_model = nullptr;
  context.f0_model = nullptr;
  context.input_parameters = nullptr;
}

int32_t NeQuick2_MLF2_F2_ml_layer_prepare(const NeQuick2_MLF2_time_t* const pTime,
                                          const NeQuick2_MLF2_modip_context_t* const pModip,
                                          const NeQuick2_MLF2_solar_activity_t* const pSolar_activity,
                                          const position_t* const pCurrent_position,
                                          angle_restricted_t solar_declination)
{
  context.input_parameters->update(pTime, pModip, pSolar_activity, pCurrent_position, solar_declination);

  return NEQUICK_OK;
}

int32_t NeQuick2_MLF2_F2_ml_layer_get_critical_freq_MHz(NeQuick2_MLF2_F2_layer_t* const pF2)
{
  DataType f0F2;
  auto ret = runModel(context.f0_model, InputParametersFoF2 {*context.input_parameters}, f0F2);
  if (ret != NEQUICK_OK)
  {
    return ret;
  }

  pF2->layer.critical_frequency_MHz = f0F2;

  return NEQUICK_OK;
}

int32_t NeQuick2_MLF2_F2_ml_layer_get_peak_height(NeQuick2_MLF2_F2_layer_t* const pF2)
{
  DataType hmF2;
  auto ret = runModel(context.hm_model, InputParametersHmF2 {*context.input_parameters}, hmF2);
  if (ret != NEQUICK_OK)
  {
    return ret;
  }

  pF2->layer.peak.height_km = hmF2 / 1000.;

  return NEQUICK_OK;
}
