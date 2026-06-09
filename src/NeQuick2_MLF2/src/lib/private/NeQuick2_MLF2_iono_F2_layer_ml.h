/** NeQuick2-MLF2 F2 layer Machine Learning part
 *
 * @author Safran Trusted 4D SAS Nantes
 * @ingroup NeQuick2-MLF2
 * @copyright Safran Trusted 4D SAS, 2026<br>
 *  This software has been released as free and open source software
 *  under the terms of the European Union Public Licence (EUPL), version 1.2<br>
 *  Questions? Submit your query at https://safran-navigation-timing.com/support-hub/skydel/
 * @file
 */
#ifndef NEQUICK2_MLF2_IONO_F2_LAYER_ML_H
#define NEQUICK2_MLF2_IONO_F2_LAYER_ML_H

#include "NeQuick2_MLF2_iono_F2_layer.h"

extern int32_t NeQuick2_MLF2_set_F2_ml_layer_data(void* hm_model,
                                                  void* f0_model,
                                                  uint16_t year,
                                                  uint8_t month,
                                                  uint8_t day,
                                                  double_t ut,
                                                  double_t ap,
                                                  double_t kp,
                                                  double_t smoothed_f107_365);

extern void NeQuick2_MLF2_F2_ml_layer_close();

extern int32_t NeQuick2_MLF2_F2_ml_layer_prepare(const NeQuick2_MLF2_time_t* const pTime,
                                                 const NeQuick2_MLF2_modip_context_t* const pModip,
                                                 const NeQuick2_MLF2_solar_activity_t* const pSolar_activity,
                                                 const position_t* const pCurrent_position,
                                                 angle_restricted_t solar_declination);

extern int32_t NeQuick2_MLF2_F2_ml_layer_get_critical_freq_MHz(NeQuick2_MLF2_F2_layer_t* const pF2);

extern int32_t NeQuick2_MLF2_F2_ml_layer_get_peak_height(NeQuick2_MLF2_F2_layer_t* const pF2);

#endif // NEQUICK2_MLF2_IONO_F2_LAYER_ML_H
