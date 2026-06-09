/** Modified Dip Latitude grid.<br>
 *
 * The MODIP grid allows to estimate MODIP [deg] at a given point
 * (latitude, longitude) by interpolation.
 * This grid is used within NeQuick to compute MODIP at a given point
 * interpolating with the 4x4-points grid surrounding the desired element.
 * The MODIP grid file contains the values of MODIP (expressed in degrees) on a geocentric
 * grid from 90&deg;S to 90&deg;N with a 1-degree step in latitude and from
 * 180&deg;W to 180&deg;E with a 2-degree in longitude.
 * For computational purposes, it is wrapped around including as first column
 * the values of 170&deg;E (i.e. 190&deg;W) and in the last column the values of 170&deg;W
 * (i.e. 190&deg;E), also there is an extra first and last rows phased 180 degrees
 * in longitude to wrap the poles around.<BR>
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
#ifndef NEQUICK2_MLF2_MODIP_GRID_H
#define NEQUICK2_MLF2_MODIP_GRID_H

#include <math.h>
#include <stdbool.h>

#include "NeQuickG_JRC_coordinates.h"

/** Number of grid points for the latitude */
#define NEQUICK_2_MLF2_MODIP_GRID_LAT_POINTS_COUNT (184)

/** Number of grid points for the longitude */
#define NEQUICK_2_MLF2_MODIP_GRID_LONG_POINTS_COUNT (184)

/** Size of longitude step in Modip grid. */
#define NEQUICK_2_MLF2_MODIP_GRID_LONG_STEP_DEGREE (2)

/** Size of latitude step in Modip grid. */
#define NEQUICK_2_MLF2_MODIP_GRID_LAT_STEP_DEGREE (1)

/** Modip grid loaded from file */
typedef struct NeQuick2_MLF2_modip_grid_st {
  /** array of pointsL the grid */
  double_t corrected_modip_degree \
    [NEQUICK_2_MLF2_MODIP_GRID_LAT_POINTS_COUNT] \
    [NEQUICK_2_MLF2_MODIP_GRID_LONG_POINTS_COUNT];
  /** loaded from file i.e. is valid? */
  bool is_loaded;
} NeQuick2_MLF2_modip_grid_t;

/** Modip grid interpolation, See 2.5.4.3.
 *
 * @param[in] pPosition location at which the modip is required
 * @param[out] pModip_degree modip in degrees
 */
extern void NeQuick2_MLF2_modip_grid_interpolate(
  const position_t* const pPosition,
  double_t *pModip_degree);

extern NeQuick2_MLF2_modip_grid_t NeQuick2_MLF2_loaded_MODIP_grid;

extern int32_t NeQuick2_MLF2_load_MODIP_grid(const char *data);
extern void NeQuick2_MLF2_clear_loaded_MODIP_grid();
extern bool NeQuick2_MLF2_is_loaded_MODIP_grid_valid();

#endif // NEQUICK2_MLF2_MODIP_GRID_H
