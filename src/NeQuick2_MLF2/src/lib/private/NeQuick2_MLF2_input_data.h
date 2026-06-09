/** NeQuick2-MLF2 input data for ray and time
 *
 * In order to compute the slant TEC along a ray path,
 * the NeQuick G model requires information on the properties of
 * that ray path (start and end points), and
 * an indication of the time at which the values are required:
 *
 *  - &phi;<SUB>1</SUB> Geodetic latitude from receiver
 *  - &lambda;<SUB>1</SUB> Geodetic longitude from receiver
 *  - h<SUB>1</SUB> Geodetic height from receiver
 *  - &phi;<SUB>2</SUB> Geodetic latitude from satellite
 *  - &lambda;<SUB>2</SUB> Geodetic longitude from satellite
 *  - h<SUB>2</SUB> Geodetic height from satellite
 *  - UT time
 *  - Month
 *
 * See 2.3 Inputs and Outputs
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
#ifndef NEQUICK2_MLF2_INPUT_DATA_H
#define NEQUICK2_MLF2_INPUT_DATA_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "NeQuickG_JRC_coordinates.h"
#include "NeQuick2_MLF2_time.h"

/** NeQuick2-MLF2 input data for ray and time */
typedef struct input_data_st {
  /** time */
  NeQuick2_MLF2_time_t time;
  /** station position */
  position_t station_position;
  /** satellite position */
  position_t satellite_position;
} input_data_t;

/** Convert the input data to km units
 * @param[in, out] pInput_data input data
 */
extern void input_data_to_km(
  input_data_t* const pInput_data);

/** Convert the input data to meter units
 * @param[in, out] pInput_data input data
 */
extern void input_data_to_meters(
  input_data_t* const pInput_data);

/** Print the input data in the standard output
 * @param[in] pInput_data input data
 * @param[in] addNewLine flag to add a new line after printing ouput data
 */
extern void input_data_to_std_output(
  const input_data_t* const pInput_data,
  bool addNewLine);

/** Print the input data
 * @param[in] pInput_data input data
 * @param[in, out] pFile file
 * @param[in] pFormat printf format parameter
 */
extern void input_data_to_output(
  const input_data_t* const pInput_data,
  FILE* const pFile,
  const char* const pFormat);

#endif // NEQUICK2_MLF2_INPUT_DATA_H
