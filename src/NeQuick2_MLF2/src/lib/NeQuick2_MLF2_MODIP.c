/** Modified Dip Latitude.<br>
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
#include "NeQuick2_MLF2_MODIP.h"

#include <assert.h>

/**
 * Note that the latitude should be between +- 90 degrees.
 * The extreme cases where the latitude is 90 degrees or -90
 * degrees are processed seperately as follows:
 * If latitude is 90 degrees, modip = 90
 * If latitude is -90 degrees, modip = -90
*/
static bool handle_special_lat_cases(
  NeQuick2_MLF2_modip_context_t* const pContext,
  const position_t* const pPosition) {
  if (pPosition->latitude.degree <=
      NEQUICK_G_JRC_LAT_MIN_VALUE_DEGREE) {
    pContext->modip_degree = NEQUICK_G_JRC_LAT_MIN_VALUE_DEGREE;
    return true;
  } else if (pPosition->latitude.degree >=
      NEQUICK_G_JRC_LAT_MAX_VALUE_DEGREE) {
    pContext->modip_degree = NEQUICK_G_JRC_LAT_MAX_VALUE_DEGREE;
    return true;
  } else {
    return false;
  }
}

void NeQuick2_MLF2_modip_get(
  NeQuick2_MLF2_modip_context_t* const pContext,
  const position_t* const pPosition) {

  // Special cases: set modip and return.
  if (handle_special_lat_cases(pContext, pPosition)) {
    return;
  }

  // otherwise interpolate using the grid
  NeQuick2_MLF2_modip_grid_interpolate(
    pPosition, &pContext->modip_degree);
}
