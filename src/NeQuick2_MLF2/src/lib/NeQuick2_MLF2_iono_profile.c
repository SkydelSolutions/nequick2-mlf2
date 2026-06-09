/** NeQuick2-MLF2 inospheric profile
 *
 * @author Angela Aragon-Angel (maria-angeles.aragon@ec.europa.eu)
 * @ingroup NeQuick2-MLF2
 * @copyright Joint Research Centre (JRC), 2019<br>
 *  This software has been released as free and open source software
 *  under the terms of the European Union Public Licence (EUPL), version 1.<br>
 *  Questions? Submit your query at https://www.gsc-europa.eu/contact-us/helpdesk
 *
 *  This file has been modified by Safran Trusted 4D, 2026
 * @file
 */
#include "NeQuick2_MLF2_iono_profile.h"

#include <assert.h>

#include "NeQuick2_MLF2_iono_E_layer.h"
#include "NeQuick2_MLF2_iono_F1_layer.h"
#include "NeQuick2_MLF2_iono_F2_layer.h"
#include "NeQuickG_JRC_math_utils.h"
#include "NeQuick2_MLF2_iono_F2_layer_ml.h"

#define NEQUICK_G_JRC_IONO_PEAK_AMPLITUDE_ITERATION_COUNT (5)

int32_t NeQuick2_MLF2_iono_profile_init(
  NeQuick2_MLF2_iono_profile_t * const pProfile) {

  NeQuick2_MLF2_E_layer_init(&pProfile->E);

  return NEQUICK_OK;
}

static double_t iono_profile_get_amplitude_of_peak(
  const peak_t* const pPeak,
  double_t height_km) {

  double_t thickness_param =
    (pPeak->height_km > height_km) ?
    pPeak->thickness.bottom_km :
    pPeak->thickness.top_km;

  double_t electron_density =
    NeQuickG_exp((height_km - pPeak->height_km) /
                  thickness_param);

  electron_density =
    (pPeak->amplitude * electron_density /
      NeQuickG_square(electron_density + 1.0));

  return NEQUICK_2_MLF2_IONO_LAYER_GET_PEAK_AMPLITUDE(electron_density);
}

static int32_t iono_profile_get_critical_freqs(
  NeQuick2_MLF2_iono_profile_t * const pProfile,
  const NeQuick2_MLF2_time_t* const pTime,
  const NeQuick2_MLF2_modip_context_t* const pModip,
  const NeQuick2_MLF2_solar_activity_t* const pSolar_activity,
  const position_t * const pCurrent_position) {

  NeQuick2_MLF2_E_layer_get_critical_freq_MHz(
    &pProfile->E,
    pTime,
    pSolar_activity,
    pCurrent_position);

  NeQuick2_MLF2_F2_ml_layer_prepare(
    pTime,
    pModip,
    pSolar_activity,
    pCurrent_position,
    pProfile->E.solar_declination);

  int32_t ret = NeQuick2_MLF2_F2_layer_get_critical_freq_MHz(
    &pProfile->F2);
  if (ret != NEQUICK_OK) {
    return ret;
  }

  NeQuick2_MLF2_F1_layer_get_critical_freq_MHz(
    &pProfile->F1,
    pProfile->E.layer.critical_frequency_MHz,
    pProfile->F2.layer.critical_frequency_MHz);

  return NEQUICK_OK;
}

static void iono_profile_get_peak_heights(
  NeQuick2_MLF2_iono_profile_t * const pProfile) {

  NeQuick2_MLF2_E_layer_get_peak_height(&pProfile->E);

  NeQuick2_MLF2_F2_layer_get_peak_height(
    &pProfile->F2,
    pProfile->E.layer.critical_frequency_MHz);

  NeQuick2_MLF2_F1_layer_get_peak_height(
    &pProfile->F1,
    pProfile->E.layer.peak.height_km,
    pProfile->F2.layer.peak.height_km);
}

static void iono_profile_get_peak_thicknesses(
  NeQuick2_MLF2_iono_profile_t * const pProfile) {

  NeQuick2_MLF2_F2_layer_get_peak_thickness(
    &pProfile->F2);

  NeQuick2_MLF2_F1_layer_get_peak_thickness(
    &pProfile->F1,
    pProfile->E.layer.peak.height_km,
    pProfile->F2.layer.peak.height_km);

  NeQuick2_MLF2_E_layer_get_peak_thickness(
    &pProfile->E,
    pProfile->F1.peak.thickness.bottom_km);
}


static double_t iono_profile_get_peak_amp_substracting_layer(
  const peak_t * const pPeak,
  const peak_t * const pOther_layer_peak) {
  double_t other_layer_contribution =
    iono_profile_get_amplitude_of_peak(pOther_layer_peak, pPeak->height_km);

  return (
    NEQUICK_2_MLF2_IONO_LAYER_GET_PEAK_AMPLITUDE(pPeak->electron_density) -
    other_layer_contribution);
}

static void NeQuick2_MLF2_iono_profile_get_peak_amplitudes(
  NeQuick2_MLF2_iono_profile_t * const pProfile) {

  NeQuick2_MLF2_F2_layer_get_peak_amplitude(&pProfile->F2);

  peak_t* const pE_peak = &pProfile->E.layer.peak;
  peak_t* const pF1_peak = &pProfile->F1.peak;
  peak_t* const pF2_peak = &pProfile->F2.layer.peak;

  double_t E_peak_amplitude_substracting_F2 =
    iono_profile_get_peak_amp_substracting_layer(pE_peak, pF2_peak);

  if (NEQUICK_2_MLF2_F1_LAYER_IS_PEAK_PRESENT(pProfile->F1)) {

    double_t F1_peak_amplitude_substracting_F2 =
      iono_profile_get_peak_amp_substracting_layer(pF1_peak, pF2_peak);

    pE_peak->amplitude =
        NEQUICK_2_MLF2_IONO_LAYER_GET_PEAK_AMPLITUDE(pE_peak->electron_density);

    for (
      size_t i = 0; i < NEQUICK_G_JRC_IONO_PEAK_AMPLITUDE_ITERATION_COUNT; i++) {

      // F1
      {
        double_t amplitude_E_at_F1_height =
        iono_profile_get_amplitude_of_peak(pE_peak, pF1_peak->height_km);

        pF1_peak->amplitude =
          F1_peak_amplitude_substracting_F2 -
          amplitude_E_at_F1_height;

        pF1_peak->amplitude =
          NeQuickG_func_join(
            pF1_peak->amplitude,
            NEQUICK_2_MLF2_IONO_F1_LAYER_AMPLITUDE_CONSTANT *
            pF1_peak->electron_density,
            1.0,
            pF1_peak->amplitude -
            (NEQUICK_2_MLF2_IONO_F1_LAYER_AMPLITUDE_CONSTANT *
             pF1_peak->electron_density));
      }

      // E
      pE_peak->amplitude =
        E_peak_amplitude_substracting_F2 -
        iono_profile_get_amplitude_of_peak(pF1_peak, pE_peak->height_km);
    }
  } else {
    pF1_peak->amplitude = 0.0;
    pE_peak->amplitude = E_peak_amplitude_substracting_F2;
  }

  pE_peak->amplitude =
    NeQuickG_func_join(
      pE_peak->amplitude,
      NEQUICK_2_MLF2_IONO_E_LAYER_AMPLITUDE_CONSTANT_1,
      NEQUICK_2_MLF2_IONO_E_LAYER_AMPLITUDE_WITH_TRANSITION_REGION,
      pE_peak->amplitude -
      NEQUICK_2_MLF2_IONO_E_LAYER_AMPLITUDE_CONSTANT_2);
}

static void iono_profile_get_peaks(NeQuick2_MLF2_iono_profile_t * const pProfile) {

  iono_profile_get_peak_heights(pProfile);

  iono_profile_get_peak_thicknesses(pProfile);

  NeQuick2_MLF2_iono_profile_get_peak_amplitudes(pProfile);
}

static void iono_profile_get_exosphere_contribution(
  NeQuick2_MLF2_iono_profile_t * const pProfile,
  const NeQuick2_MLF2_solar_activity_t* const pSolar_activity) {

  NeQuick2_MLF2_F2_layer_exosphere_adjust(
    &pProfile->F2,
    pSolar_activity);
}

int32_t NeQuick2_MLF2_iono_profile_get(
  NeQuick2_MLF2_iono_profile_t * const pProfile,
  const NeQuick2_MLF2_time_t* const pTime,
  NeQuick2_MLF2_modip_context_t* const pModip,
  const NeQuick2_MLF2_solar_activity_t* const pSolar_activity,
  const position_t * const pCurrent_position) {

  NeQuick2_MLF2_modip_get(pModip, pCurrent_position);

  int32_t ret = iono_profile_get_critical_freqs(
    pProfile,
    pTime,
    pModip,
    pSolar_activity,
    pCurrent_position);
  if (ret != NEQUICK_OK) {
    return ret;
  }

  iono_profile_get_peaks(pProfile);

  iono_profile_get_exosphere_contribution(
    pProfile,
    pSolar_activity);

  return NEQUICK_OK;
}

#undef NEQUICK_G_JRC_IONO_PEAK_AMPLITUDE_ITERATION_COUNT
