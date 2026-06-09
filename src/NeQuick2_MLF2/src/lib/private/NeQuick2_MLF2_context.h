/**
 * NeQuick2-MLF2 library context.
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
#ifndef NEQUICK2_MLF2_CONTEXT_H
#define NEQUICK2_MLF2_CONTEXT_H

#include "NeQuick2_MLF2_input_data.h"
#include "NeQuick2_MLF2_iono_profile.h"
#include "NeQuick2_MLF2_MODIP.h"
#include "NeQuick2_MLF2_solar_activity.h"
#include "NeQuickG_JRC_ray.h"

/** This structure contains the internal context
 * of the library.
 */
typedef struct NeQuick2_MLF2_context_st {
  /** modip contex.*/
  NeQuick2_MLF2_modip_context_t modip;
  /** solar activity contex.*/
  NeQuick2_MLF2_solar_activity_t solar_activity;
  /** ionospheric profile contex.*/
  NeQuick2_MLF2_iono_profile_t profile;
  /** calculated ray contex.*/
  ray_context_t ray;
  /** input data contex.*/
  input_data_t input_data;
} NeQuick2_MLF2_context_t;

#endif // NEQUICK2_MLF2_CONTEXT_H
