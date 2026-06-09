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
#ifndef NEQUICK2_MLF2_IONO_F2_LAYER_H
#define NEQUICK2_MLF2_IONO_F2_LAYER_H

#include <math.h>

#include "NeQuick2_MLF2_input_data.h"
#include "NeQuickG_JRC_iono_profile_types.h"
#include "NeQuick2_MLF2_MODIP.h"
#include "NeQuick2_MLF2_solar_activity.h"

/** F2 layer profile context */
typedef struct NeQuick2_MLF2_F2_layer_st {
  /** generic ionospheric layer profile context */
  layer_t layer;
  /** Transmission factor M(3000)F2
   * The ratio of the maximum usable frequency at
   *  a distance of 3000 km to the F2 layer critical frequency.
   */
  double_t trans_factor;
} NeQuick2_MLF2_F2_layer_t;

/** Get F2 layer critical frequency f0F2 in MHz.
 * For the calculation of foF2, the associated Machine Learning model is used.
 *
 * @param[in, out] pContext F2 layer profile context
 */
extern int32_t NeQuick2_MLF2_F2_layer_get_critical_freq_MHz(
  NeQuick2_MLF2_F2_layer_t* const pContext);

/** Get F2 layer maximum density height hmF2 [km] and Transmission factor M(3000)F2.
 * For the calculation of hmF2, the associated Machine Learning model is used.
 * For the calculation of M(3000)F2, an inversed method of Dudeney is used,
 * from already computed f0F2, hmF2 and f0E.
 *
 * @param[in, out] pContext F2 layer profile context
 * @param[in] critical_freq_E_layer_MHz E layer critical frequency f0E in MHz
 */
extern void NeQuick2_MLF2_F2_layer_get_peak_height(
  NeQuick2_MLF2_F2_layer_t* const pContext,
  const double_t critical_freq_E_layer_MHz);

/** Get F2 layer peak thickness see 2.5.5.7, Eq 85
 *
 * @param[in, out] pContext F2 layer profile context
 */
extern void NeQuick2_MLF2_F2_layer_get_peak_thickness(
  NeQuick2_MLF2_F2_layer_t* const pContext);

/** Get F2 layer peak amplitude see 2.5.5.8, Eq 90
 *
 * @param[in, out] pContext F2 layer profile context
 */
extern void NeQuick2_MLF2_F2_layer_get_peak_amplitude(
  NeQuick2_MLF2_F2_layer_t* const pContext);

/** Compute the topside thickness parameter H0 [km] see 2.6.2
 *
 * @param[in, out] pContext F2 layer profile context
 * @param[in] pSolar_activity solar activity
 */
extern void NeQuick2_MLF2_F2_layer_exosphere_adjust(
  NeQuick2_MLF2_F2_layer_t* const pContext,
  const NeQuick2_MLF2_solar_activity_t* const pSolar_activity);

#endif // NEQUICK2_MLF2_2_IONO_F2_LAYER_H
