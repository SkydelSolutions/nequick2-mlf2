/** Solar Radio Flux at 10.7cm
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
#include "NeQuick2_MLF2_solar_activity.h"

#include <assert.h>
#include <stdbool.h>

#include "NeQuickG_JRC_error.h"
#include "NeQuickG_JRC_macros.h"

#define NEQUICK_G_AZ_EQUALITY_EPSILON (1.0e-10)

/** 3.1. This default value represents the lowest Solar Flux value in average
 *  conditions that NeQuick is expected to operate on.
 *  In terms of the analytical expression relating 12-month running Sun
 *  Spot Number and Solar Flux, it corresponds to a Sun Spot Number of 0.
 *  This value is considered adequate when no other solution is available,
 *  while still being able to correct for a significant contribution of
 *  the ionospheric delay error.
 */
#define NEQUICK_G_AZ_DEFAULT_VALUE_SFU (ITU_R_P_371_8_LOWER_SOLAR_FLUX_IN_SFU)

static void check_boundaries(
  double_t* const flux) {
  if (*flux < NEQUICK_2_MLF2_F107_MIN_VALUE_SFU) {
    *flux = NEQUICK_2_MLF2_F107_MIN_VALUE_SFU;
  } else if (*flux > NEQUICK_2_MLF2_F107_MAX_VALUE_SFU) {
    *flux = NEQUICK_2_MLF2_F107_MAX_VALUE_SFU;
  }
}

static double_t
NeQuick2_MLF2_solar_activity_get_effective_ionisation_level_in_sfu(
  double_t flux) {
  if (flux <  1.0e-7) {
    /**
     *   In those cases, a default value shall be used for correction in
     *   the receiver:
     *   ai0 = NEQUICK_G_AZ_DEFAULT_VALUE; ai1 = ai2 = 0
     */
    return NEQUICK_G_AZ_DEFAULT_VALUE_SFU;
  }

  {
    check_boundaries(&flux);
    return flux;
  }
}

bool NeQuick2_MLF2_solar_activity_is_effective_ionisation_level_equal(
  double_t op1,
  double_t op2) {
  return THRESHOLD_COMPARE(op1, op2,
          NEQUICK_G_AZ_EQUALITY_EPSILON);
}

int32_t NeQuick2_MLF2_solar_activity_set (
  NeQuick2_MLF2_solar_activity_t* const pContext,
  double_t flux) {
  // F10.7
  pContext->solar_radio_flux_sfu =
      NeQuick2_MLF2_solar_activity_get_effective_ionisation_level_in_sfu(flux);

  // R12
  pContext->mean_sun_spot_count =
    NeQuick2_MLF2_F107_solar_activity_get_sun_spot_effective_count(
      pContext->solar_radio_flux_sfu);

  return NEQUICK_OK;
}

#undef NEQUICK_G_AZ_EQUALITY_EPSILON
#undef NEQUICK_G_AZ_DEFAULT_VALUE_SFU
