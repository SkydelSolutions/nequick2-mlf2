/** NeQuick2-MLF2 inospheric profile
 *
 * NeQuick is a profiler that makes use of three profile anchor points:
 *  - E layer peak (at a fixed height of 120 km),
 *  - F1 peak,
 *  - F2 peak<br>
 * where E, F1 and F2 are different layers of the ionosphere, as previously introduced.
 * To model the anchor points the model employs ionosonde parameters
 *  - foE, foF1, foF2 (critical frequencies) and
 *  - M(3000)F2 (transmission factor).
 *
 * The model is constituted by two major components:
 *  - The bottom side model for the height region below the peak of the F2-layer, which consists on the
 * superposition of three Epstein layers which peak at the anchor points.
 * This is a modified version of the Di Giovanni-Radicella model based on the ionospheric characteristics
 * foE, foF1, foF2 and M(3000)F2.
 * For foE derivation, a modified formulation of that due to John Titheridge is selected and foF1 is selected
 * as being equal to 1.4*foE during daytime and zero during night-time, respectively.
 * For the calculation of foF2, the associated Machine Learning model is used.
 *
 *  - The topside model for the height region above the F2-layer peak.
 * The topside of NeQuick is a semi-Epstein layer with a height dependent thickness parameter B
 * through a new parameter H. A correction factor adjusts vertical TEC values to take into account
 *  exosphere electron density in a simple manner.
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
#ifndef NEQUICK2_MLF2_IONO_PROFILE_H
#define NEQUICK2_MLF2_IONO_PROFILE_H

#include "NeQuick2_MLF2_iono_E_layer.h"
#include "NeQuick2_MLF2_iono_F2_layer.h"
#include "NeQuickG_JRC_iono_profile_types.h"
#include "NeQuick2_MLF2_MODIP.h"
#include "NeQuick2_MLF2_time.h"

/** From critical frequency, calculates the associated electron density.
 * See Eqs. 36, 48, 39 and 77, see F.2.9.4
 */
#define NEQUICK_2_MLF2_FREQ_TO_NE_FACTOR_D (0.124)

/** From critical frequency, calculates the associated electron density, using
 *  N[m<SUP>-3</SUP>] = 0.124 * f[MHz]<SUP>2</SUP>
 *
 * @param[in] _crit_freq_MHz The peak plasma frequency for the layer
 * @return The calculated electron density (in 10<SUP>11</SUP> m<SUP>-3</SUP>)
 */
#define NEQUICK_2_MLF2_IONO_GET_e_DENSITY(_crit_freq_MHz) \
  (NEQUICK_2_MLF2_FREQ_TO_NE_FACTOR_D * _crit_freq_MHz * _crit_freq_MHz)

/** Get peak amplitude from the electron density Eqs. 90, 92, 93, 94 and 96
 *
 * @param[in] _e_density the electron density in 10<SUP>11</SUP> m<SUP>-3</SUP>
 * @return the peak amplitude in 10<SUP>11</SUP> m<SUP>-3</SUP>
 */
#define NEQUICK_2_MLF2_IONO_LAYER_GET_PEAK_AMPLITUDE(_e_density) \
 (_e_density * 4.0)

/** NeQuick2-MLF2 inospheric profile context */
typedef struct NeQuick2_MLF2_iono_profile_st {
  /** E layer context */
  NeQuick2_MLF2_E_layer_t E;
  /** F1 layer context */
  layer_t F1;
  /** F2 layer context */
  NeQuick2_MLF2_F2_layer_t F2;
} NeQuick2_MLF2_iono_profile_t;

/** Initializes the ionospheric profile context
 * @param[out] pContext ionospheric profile context
 */
extern int32_t NeQuick2_MLF2_iono_profile_init(
  NeQuick2_MLF2_iono_profile_t* const pContext);

/** Get the ionospheric profile
 *
 * @param[out] pContext ionospheric profile context
 * @param[in] pTime indication of the time at which the profile is required
 * @param[in] pModip MODIP context
 * @param[in] pSolar_activity solar activity
 * @param[in] pCurrent_position location at which the profile is required
 */
extern int32_t NeQuick2_MLF2_iono_profile_get(
  NeQuick2_MLF2_iono_profile_t* const pContext,
  const NeQuick2_MLF2_time_t* const pTime,
  NeQuick2_MLF2_modip_context_t* const pModip,
  const NeQuick2_MLF2_solar_activity_t* const pSolar_activity,
  const position_t * const pCurrent_position);

#endif // NEQUICK2_MLF2_IONO_PROFILE_H
