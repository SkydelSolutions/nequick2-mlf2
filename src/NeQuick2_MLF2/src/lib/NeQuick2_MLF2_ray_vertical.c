/** NeQuick2-MLF2 slant vertical ray
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
#include "NeQuick2_MLF2_ray_vertical.h"

#include "NeQuick2_MLF2_electron_density.h"

static position_t get_current_position(const ray_context_t* const pRay) {
  position_t current_position;

  current_position.latitude = pRay->receiver_position.latitude;
  current_position.longitude = pRay->receiver_position.longitude;

  // not used for profile...
  current_position.height = pRay->satellite_position.height;
  current_position.height_units = pRay->satellite_position.height_units;

  return current_position;
}

int32_t NeQuick2_MLF2_ray_vertical_get_profile(NeQuick2_MLF2_context_t* const pContext) {

  position_t current_position = get_current_position(&pContext->ray);

  return NeQuick2_MLF2_iono_profile_get(
    &pContext->profile,
    &pContext->input_data.time,
    &pContext->modip,
    &pContext->solar_activity,
    &current_position);
}

double_t NeQuick2_MLF2_ray_vertical_get_electron_density(
  NeQuick2_MLF2_context_t* const pContext,
  const double_t height_km) {
  return NeQuick2_MLF2_electron_density_get(&pContext->profile, height_km);
}
