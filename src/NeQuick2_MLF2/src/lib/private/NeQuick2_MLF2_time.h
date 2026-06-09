/** NeQuick2-MLF2 Time constants and routines
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
#ifndef NEQUICK2_MLF2_TIME_H
#define NEQUICK2_MLF2_TIME_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "NeQuickG_JRC_coordinates.h"

/** Number of hours in a day */
#define NEQUICK_2_MLF2_TIME_DAY_IN_HOURS (24.0)
/** A half day in hours */
#define NEQUICK_2_MLF2_TIME_HALF_DAY_IN_HOURS \
  (NEQUICK_2_MLF2_TIME_DAY_IN_HOURS / 2.0)

/** Number of degrees per hour */
#define NEQUICK_2_MLF2_TIME_DEGREE_PER_HOUR (15.0)

/** Index for January */
#define NEQUICK_2_MLF2_MONTH_JANUARY (1)
/** Index for February */
#define NEQUICK_2_MLF2_MONTH_FEBRUARY (2)
/** Index for March */
#define NEQUICK_2_MLF2_MONTH_MARCH (3)
/** Index for April */
#define NEQUICK_2_MLF2_MONTH_APRIL (4)
/** Index for May */
#define NEQUICK_2_MLF2_MONTH_MAY (5)
/** Index for June */
#define NEQUICK_2_MLF2_MONTH_JUNE (6)
/** Index for July */
#define NEQUICK_2_MLF2_MONTH_JULY (7)
/** Index for August */
#define NEQUICK_2_MLF2_MONTH_AUGUST (8)
/** Index for September */
#define NEQUICK_2_MLF2_MONTH_SEPTEMBER (9)
/** Index for October */
#define NEQUICK_2_MLF2_MONTH_OCTOBER (10)
/** Index for November */
#define NEQUICK_2_MLF2_MONTH_NOVEMBER (11)
/** Index for December */
#define NEQUICK_2_MLF2_MONTH_DECEMBER (12)

#define NEQUICK_2_MLF2_MONTHS_COUNT (12)

/** Nequick model uses UT and month */
typedef struct NeQuick2_MLF2_time_st {
  /** UT time in hours */
  double_t utc;
  /** Month [#NEQUICK_2_MLF2_MONTH_JANUARY-#NEQUICK_2_MLF2_MONTH_DECEMBER] */
  uint8_t month;
  uint16_t doy;
} NeQuick2_MLF2_time_t;

/** Initializes time context to zero
 *
 * @param[out] pTime time context
 */
extern void NeQuick2_MLF2_time_init(
  NeQuick2_MLF2_time_t* const pTime);

/** Sets time context
 *
 * @param[out] pTime time context
 * @param[in] month [#NEQUICK_2_MLF2_MONTH_JANUARY-#NEQUICK_2_MLF2_MONTH_DECEMBER]
 * @param[in] utc UT in hours
 */
extern int32_t NeQuick2_MLF2_time_set(
  NeQuick2_MLF2_time_t* const pTime,
  const uint_fast8_t month,
  const double_t utc,
  const uint16_t doy);

/** Get local time see 2.5.4.1.
 *
 * @param[in] pTime time context
 * @param[in] longitude_degree &lambda; in degrees
 * @return local time LT
 */
extern double_t NeQuick2_MLF2_time_get_local(
  const NeQuick2_MLF2_time_t* const pTime,
  double_t longitude_degree);

/** Checks month equality
 *
 * @param[in] op1 month 1
 * @param[in] op2 month 2
 * @return true if equal
 */
#define NEQUICK_2_MLF2_TIME_MONTH_EQUAL(op1, op2) \
  (op1 == op2)

/** Checks UT equality using a threshold comparison approach with &epsilon; = #NEQUICK_2_MLF2_TIME_UTC_EPSILON
 *
 * @param[in] ut1 Universal time 1
 * @param[in] ut2 Universal time 2
 * @return true if equal
 */
extern bool NeQuick2_MLF2_time_utc_is_equal(
  double_t ut1,
  double_t ut2);

#endif // NEQUICK2_MLF2_TIME_H
