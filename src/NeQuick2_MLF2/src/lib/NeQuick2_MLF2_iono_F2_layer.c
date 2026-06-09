/** NeQuick2-MLF2 F2 layer profile
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
#include "NeQuick2_MLF2_iono_F2_layer.h"

#include <assert.h>
#include <stdlib.h>

#include "NeQuick2_MLF2_iono_F2_layer_ml.h"
#include "NeQuick2_MLF2_iono_profile.h"
#include "NeQuick2_MLF2_iono_E_layer.h"
#include "NeQuickG_JRC_geometry.h"
#include "NeQuickG_JRC_macros.h"
#include "NeQuickG_JRC_math_utils.h"

/** Transmission factor must be at least 1.0
 * otherwise a square root could be calculated
 * with a negative operand.
 * TBD. (undocumented feature and value)
 */
#define NEQUICKG_JRC_IONO_F2_TRANS_FACTOR_MINIMUM_VALUE (1.0)
#define NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_MAXIMUM_VALUE (4.5)

#define NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_1 (1490.0)
#define NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_2 (0.0196)
#define NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_3 (1.2967)
#define NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_4 (-0.012)
#define NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_5 (0.253)
#define NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_6 (1.215)
#define NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_7 (-176.0)
#define NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_JOIN_CONST (20.0)
#define NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_JOIN_CLIP_CONST (1.75)

#define NEQUICK_G_JRC_IONO_F2_LAYER_THICKNESS_BOTTOM_FACTOR (0.385)

#define NEQUICKG_JRC_IONO_F2_e_DENSITY_GRAD_CONST_1 (0.01)
#define NEQUICKG_JRC_IONO_F2_e_DENSITY_GRAD_CONST_2 (-3.467)
#define NEQUICKG_JRC_IONO_F2_e_DENSITY_GRAD_CONST_3 (0.857)
#define NEQUICKG_JRC_IONO_F2_e_DENSITY_GRAD_CONST_4 (2.02)

/* Constants used to calculate the F2 topside thickness */
#define NEQUICKG_JRC_IONO_F2_THICKNESS_TOP_CONST_1 (150.0)
#define NEQUICKG_JRC_IONO_F2_THICKNESS_TOP_CONST_2 (100.0)
#define NEQUICKG_JRC_IONO_F2_THICKNESS_TOP_CONST_3 (0.041163)
#define NEQUICKG_JRC_IONO_F2_THICKNESS_TOP_CONST_4 (0.183981)
#define NEQUICKG_JRC_IONO_F2_THICKNESS_TOP_CONST_5 (1.424472)

/* Constants used to calculate the NeQuick 2 shape factor */
#define NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_1 (3.22)
#define NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_2 (0.0538)
#define NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_3 (0.00664)
#define NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_4 (0.113)
#define NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_5 (0.00257)

/* Constants used to calculate the NeQuick2-MLF2 estimated transmission factor */
#define NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_START_VALUE (3.0)
#define NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_DENOMINATOR_MINIMUM_VALUE (0.01)
#define NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_MINIMUM_E_LAYER_FREQ (1e-30)
#define NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_TOLERANCE (0.001)


/** Estimate M(3000)F2 from hmF2 using inverse of standard ionospheric
 * peak height formula.
 */
static double_t get_estimated_transmission_factor(
  const NeQuick2_MLF2_F2_layer_t* const pF2,
  const double_t critical_freq_E_layer_MHz) {
  double_t trans_factor = NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_START_VALUE;

  size_t i;
  for (i = 0;
    i < (size_t)NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_JOIN_CONST;
    i++) {
    double_t square = trans_factor * trans_factor;

    double_t denominator = NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_3 * square - 1.0;

    if(denominator <= 0.) {
      denominator = NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_DENOMINATOR_MINIMUM_VALUE;
    }

    double_t mf = trans_factor *
      sqrt((NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_2 * square + 1.0)
        / denominator);

    double_t dM = NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_4;
    if(critical_freq_E_layer_MHz >=
      NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_MINIMUM_E_LAYER_FREQ) {
      double_t freq_ratio = max(pF2->layer.critical_frequency_MHz / critical_freq_E_layer_MHz,
                                NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_JOIN_CLIP_CONST);

      dM = NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_5
          / (freq_ratio - NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_6)
          + NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_4;
    }

    double_t new_trans_factor = NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_1 * mf
        / (pF2->layer.peak.height_km - NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_7)
        - dM;

    if(fabs(new_trans_factor - trans_factor) < NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_TOLERANCE) {
      break;
    }

    trans_factor = (trans_factor + new_trans_factor) / 2.0;
  }

  trans_factor = max(min(trans_factor, NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_MAXIMUM_VALUE),
                     NEQUICKG_JRC_IONO_F2_TRANS_FACTOR_MINIMUM_VALUE);

  return trans_factor;
}

int32_t NeQuick2_MLF2_F2_layer_get_critical_freq_MHz(
  NeQuick2_MLF2_F2_layer_t * const pF2) {
  NeQuick2_MLF2_F2_ml_layer_get_critical_freq_MHz(pF2);

  pF2->layer.peak.electron_density =
    NEQUICK_2_MLF2_IONO_GET_e_DENSITY(
      pF2->layer.critical_frequency_MHz);

  return NEQUICK_OK;
}

void NeQuick2_MLF2_F2_layer_get_peak_height(
  NeQuick2_MLF2_F2_layer_t* const pF2,
  const double_t critical_freq_E_layer_MHz) {
  NeQuick2_MLF2_F2_ml_layer_get_peak_height(pF2);

  pF2->trans_factor = get_estimated_transmission_factor(pF2, critical_freq_E_layer_MHz);
}

void NeQuick2_MLF2_F2_layer_get_peak_thickness(
  NeQuick2_MLF2_F2_layer_t* const pF2) {
  pF2->layer.peak.thickness.top_km = INFINITY;
  pF2->layer.peak.thickness.bottom_km =
    NEQUICK_G_JRC_IONO_F2_LAYER_THICKNESS_BOTTOM_FACTOR *
    pF2->layer.peak.electron_density;

  /* Calculate gradient of rho(e) at base of F2 layer (10^9 m^-3 km^-1)
   * re: Mosert de Gonzalez and Radicella, 1990, Adv. Space. Res., 10, 17 */
  double_t grad =
    NEQUICKG_JRC_IONO_F2_e_DENSITY_GRAD_CONST_1 *
    exp(NEQUICKG_JRC_IONO_F2_e_DENSITY_GRAD_CONST_2 +
       (NEQUICKG_JRC_IONO_F2_e_DENSITY_GRAD_CONST_3 *
         log(pF2->layer.critical_frequency_MHz *
             pF2->layer.critical_frequency_MHz)) +
       (NEQUICKG_JRC_IONO_F2_e_DENSITY_GRAD_CONST_4 *
         log(pF2->trans_factor))
       );

  pF2->layer.peak.thickness.bottom_km /= grad;
}

void NeQuick2_MLF2_F2_layer_get_peak_amplitude(
  NeQuick2_MLF2_F2_layer_t* const pF2) {
  pF2->layer.peak.amplitude =
    NEQUICK_2_MLF2_IONO_LAYER_GET_PEAK_AMPLITUDE(
      pF2->layer.peak.electron_density);
}

static double_t F2_layer_get_shape_factor(
  const NeQuick2_MLF2_F2_layer_t* const pF2,
  const NeQuick2_MLF2_solar_activity_t* const pSolar_activity) {

  double_t shape_factor =
      NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_1 - 
      (NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_2 *  pF2->layer.critical_frequency_MHz) -
      (NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_3 * pF2->layer.peak.height_km) +
      (NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_4 * 
          (pF2->layer.peak.height_km / pF2->layer.peak.thickness.bottom_km)) +
      (NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_5 * pSolar_activity->mean_sun_spot_count);

  return shape_factor;
}

void NeQuick2_MLF2_F2_layer_exosphere_adjust(
  NeQuick2_MLF2_F2_layer_t* const pF2,
  const NeQuick2_MLF2_solar_activity_t* const pSolar_activity) {

  double_t shape_factor =
    F2_layer_get_shape_factor(pF2, pSolar_activity);

  pF2->layer.peak.thickness.top_km =
    shape_factor * pF2->layer.peak.thickness.bottom_km;

  // Set to Not a Number.
  // It must be recalculated using the
  // electron density module
  pF2->layer.peak.electron_density = NAN;
}

#undef NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_START_VALUE
#undef NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_DENOMINATOR_MINIMUM_VALUE
#undef NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_MINIMUM_E_LAYER_FREQ
#undef NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_TOLERANCE
#undef NEQUICK2_MLF2_ESTIMATED_TRANS_FACTOR_MAXIMUM_VALUE

#undef NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_1
#undef NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_2
#undef NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_3
#undef NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_4
#undef NEQUICK2_MLF2_IONO_F2_SHAPE_FACTOR_CONST_5

#undef NEQUICKG_JRC_IONO_F2_THICKNESS_TOP_CONST_1
#undef NEQUICKG_JRC_IONO_F2_THICKNESS_TOP_CONST_2
#undef NEQUICKG_JRC_IONO_F2_THICKNESS_TOP_CONST_3
#undef NEQUICKG_JRC_IONO_F2_THICKNESS_TOP_CONST_4
#undef NEQUICKG_JRC_IONO_F2_THICKNESS_TOP_CONST_5

#undef NEQUICKG_JRC_IONO_F2_e_DENSITY_GRAD_CONST_1
#undef NEQUICKG_JRC_IONO_F2_e_DENSITY_GRAD_CONST_2
#undef NEQUICKG_JRC_IONO_F2_e_DENSITY_GRAD_CONST_3
#undef NEQUICKG_JRC_IONO_F2_e_DENSITY_GRAD_CONST_4

#undef NEQUICK_G_JRC_IONO_F2_LAYER_THICKNESS_BOTTOM_FACTOR

#undef NEQUICKG_JRC_IONO_F2_TRANS_FACTOR_MINIMUM_VALUE

#undef NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_1
#undef NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_2
#undef NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_3
#undef NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_4
#undef NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_5
#undef NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_6
#undef NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_CONST_7
#undef NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_JOIN_CONST
#undef NEQUICKG_JRC_IONO_F2_MAX_e_DENSITY_DUDENEY_JOIN_CLIP_CONST
