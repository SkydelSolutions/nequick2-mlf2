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
#ifndef NEQUICK2_MLF2_MODIP_H
#define NEQUICK2_MLF2_MODIP_H

#include <math.h>
#include <stdbool.h>

#include "NeQuickG_JRC_coordinates.h"
#include "NeQuick2_MLF2_MODIP_grid.h"

/** Modip context */
typedef struct NeQuick2_MLF2_modip_context_st {
  /** calculated modip in degrees */
  double_t modip_degree;
} NeQuick2_MLF2_modip_context_t;

/** Get receiver modip by grid interpolation, ITU-R P.2297-1 2.2.3.2.
 *
 * @param[in, out] pContext modip context
 * @param[in] pPosition location at which the modip is required
 */
extern void NeQuick2_MLF2_modip_get(
  NeQuick2_MLF2_modip_context_t* const pContext,
  const position_t* const pPosition);

#endif // NEQUICK2_MLF2_MODIP_H
