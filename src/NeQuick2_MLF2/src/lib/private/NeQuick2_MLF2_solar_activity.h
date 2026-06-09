/** Solar Radio Flux at 10.7cm<br>
 *
 * Note that sfu (solar flux unit) is not a SI unit but can be converted as:
 *  1 sfu = 10<SUP>-22</SUP> W/(m<SUP>2</SUP>*Hz)
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
#ifndef NEQUICK2_MLF2_SOLAR_ACTIVITY_H
#define NEQUICK2_MLF2_SOLAR_ACTIVITY_H

#include <math.h>
#include <stddef.h>

#include "ITU_R_P_371_8.h"
#include "NeQuick2_MLF2_MODIP.h"

#define NEQUICK_2_MLF2_F107_MIN_VALUE_SFU (0.0)

#define NEQUICK_2_MLF2_F107_MAX_VALUE_SFU (400.0)

/** solar activity */
typedef struct NeQuick2_MLF2_solar_activity_st {
  /** Solar Radio Flux (F10.7) in sfu */
  double_t solar_radio_flux_sfu;
  /** 12-month-running mean of sunspot number (R12) */
  double_t mean_sun_spot_count;
} NeQuick2_MLF2_solar_activity_t;

/** Set the solar activity values in the context
 *
 * @param[in, out] pSolar_activity solar activity context
 * @param[in] flux solar radio flux at 10.7cm wavelength
 */
extern int32_t NeQuick2_MLF2_solar_activity_set(
  NeQuick2_MLF2_solar_activity_t* const pSolar_activity,
  double_t flux);


/** Compares two values of the Solar Radio Flux F10.7 using a threshold based comparison approach
 * Threshold used is #NEQUICK_G_AZ_EQUALITY_EPSILON
 *
 * @param[in] F107_1 first F107 in sfu
 * @param[in] F107_2 second F107 in sfu
 * @return true if equal
 */
extern bool NeQuick2_MLF2_solar_activity_is_effective_ionisation_level_equal(
  double_t F107_1,
  double_t F107_2);

/** Compute the Mean of Sunspot Number R12 as a function of the
 * Solar Radio Flux F10.7.
 *
 * @param F107_in_sfu Solar Radio Flux F10.7 in sfu
 *
 * @return Mean of Sunspot Number R12
 */
#define NeQuick2_MLF2_F107_solar_activity_get_sun_spot_effective_count(F107_in_sfu) \
    get_solar_mean_spot_number(F107_in_sfu)

#endif // NEQUICK2_MLF2_SOLAR_ACTIVITY_H
