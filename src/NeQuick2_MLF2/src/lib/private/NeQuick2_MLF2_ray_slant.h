/** NeQuick2-MLF2 slant ray electron density
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
#ifndef NEQUICK2_MLF2_RAY_SLANT_H
#define NEQUICK2_MLF2_RAY_SLANT_H

#include <math.h>

#include "NeQuick2_MLF2_context.h"

/** This function returns electron density at the specified point along a slanted ray.
 * See F.2.8.1.<br>
 * - Adjusts position information for current position along ray
 * - Recalculates ionosphere information for new latitude and longitude (#iono_profile_get)
 * - Gets the electron density with a call to #electron_density_get
 *
 * @param[in, out] pContext NeQuick context
 * @param[in] height_km Distance of the point along the ray in km
 * @param[out] pElectron_density Maximum N<SUB>e</SUB> in electrons/m<SUP>3</SUP>
 *
 * @return on success NEQUICK_OK
 */
extern int32_t NeQuick2_MLF2_ray_slant_get_electron_density(
  NeQuick2_MLF2_context_t* const pContext,
  const double_t height_km,
  double_t* const pElectron_density);

#endif // NEQUICK2_MLF2_RAY_SLANT_H
