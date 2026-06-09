/**
 *  Ionospheric Correction Algorithm for Galileo Single Frequency Users.
 *
 *  Example implementation of the algorithm to compute ionospheric corrections based
 *  on the broadcast coefficients in the navigation message for Galileo single-frequency users.<br>
 *  The term Galileo is used to refer to the Global Navigation Satellite System (GNSS) established under
 *  the European GNSS programme.
 *
 *  <h3>Spec(s):</h3>
 *    - European GNSS (Galileo) Open Service. Ionospheric Correction Algorithm for Galileo Single Frequency Users, 1.2 September 2016
 *
 *  <h3>Hints for Usage:</h3>
 *    - Initialize the library (#NeQuick2_MLF2_library.init)
 *
 *    - Set inputs:<br>
 *       - solar activity flux (#NeQuick2_MLF2_library.set_solar_activity_flux)<br>
 *       - time (#NeQuick2_MLF2_library.set_time)<br>
 *       - receiver position (#NeQuick2_MLF2_library.set_receiver_position)<br>
 *       - satellite position (#NeQuick2_MLF2_library.set_satellite_position)<br>
 *
 *    - Get the Slant Total Electron Content (STEC) in TECU (#NeQuick2_MLF2_library.get_total_electron_content)
 *
 *    - Free resources (#NeQuick2_MLF2_library.close)
 *
 * The STEC integration routine used in this implementation is the method based on
 * Kronrod G<SUB>7</SUB>-K<SUB>15</SUB> adaptive quadrature with the default tolerances defined in the specification.
 * See section F.2.6.<br>
 * The maximum recursion level for the integration routine is #NEQUICK_G_JRC_RECURSION_LIMIT_MAX<br>
 *
 * @mainpage NeQuick2-MLF2 (Galileo)
 * @author Angela Aragon-Angel (maria-angeles.aragon@ec.europa.eu)
 * @bug No known bugs.
 * @defgroup NeQuickG_JRC NeQuick2-MLF2 [NeQuick2-MLF2]
 * @copyright Joint Research Centre (JRC), 2019<br>
 *  This software has been released as free and open source software
 *  under the terms of the European Union Public Licence (EUPL), version 1.<br>
 *  Questions? Submit your query at https://www.gsc-europa.eu/contact-us/helpdesk
 *
 *  This file has been modified by Safran Trusted 4D, 2026
 * @file
 */
#ifndef NEQUICK2_MLF2_H
#define NEQUICK2_MLF2_H

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/** This is NeQuick2-MLF2 version 1.0*/
#define NEQUICK_2_MLF2_VERSION (1.0)

/** Invalid handle */
#define NEQUICK_2_MLF2_INVALID_HANDLE (NULL)

/** Number of WGS-84 ellipsoidal coordinates:
 * latitude, longitude and height
 */
#define NEQUICK_2_MLF2_WGS_84_ELLIPSOIDAL_COORDINATES_COUNT (3)

/** Number of time parameters:
 * month and UTC
 */
#define NEQUICK_2_MLF2_TIME_PARAMETERS_COUNT (2)

/** NeQuick2-MLF2 handle */
typedef void* NeQuick2_MLF2_handle;

/** NeQuick2-MLF2 constant handle */
typedef const void* const NeQuick2_MLF2_chandle;

/** NeQuick2-MLF2 API */
struct NeQuick2_MLF2_library {
  /** NeQuick2-MLF2 library initialization
   *
   * the library context is allocated in the heap<br>
   *
   * @param[out] pHandle on success contains a valid NeQuick2-MLF2 handle,
   *  on error is set to NEQUICKG_INVALID_HANDLE
   *
   * @return on success NEQUICK_OK
   */
   int32_t (*init)(
    NeQuick2_MLF2_handle* const pHandle);

   int32_t (*load_MODIP_grid)(
     const char* data
   );

  /** NeQuick2-MLF2 library uninitialization.
   *  Free resources allocated by init.
   *
   * @param[in] NeQuick2_MLF2_handle NeQuick2-MLF2 handle
   */
  void (*close)(const NeQuick2_MLF2_handle);

  /** Sets solar activity flux
   *
   * @param[in] NeQuick2_MLF2_handle NeQuick2-MLF2 handle
   * @param[in] flux solar radio flux at 10.7cm wavelength
   *
   * @return on success NEQUICK_OK
   */
  int32_t (*set_solar_activity_flux)(
    const NeQuick2_MLF2_handle,
    double_t flux);

  /** Sets time
   *
   * @param[in] NeQuick2_MLF2_handle NeQuick2-MLF2 handle
   * @param[in] month numerical value January = 1, ..., December = 12
   * @param[in] UTC UT time (hours) range accepted [0, 24]
   *
   * @return on success NEQUICK_OK
   */
  int32_t (*set_time)(
    const NeQuick2_MLF2_handle,
    const uint8_t month,
    const double_t UTC,
    const uint16_t doy);

  /** Sets the receiver position using WGS-84 ellipsoidal coordinates
   *
   * @param[in] NeQuick2_MLF2_handle NeQuick2-MLF2 handle
   * @param[in] longitude_degree Geodetic longitude from receiver (degrees)
   * @param[in] latitude_degree Geodetic latitude from receiver (degrees) range accepted [-90, +90]
   * @param[in] height_meters Geodetic height from receiver (m)
   *
   * @return on success NEQUICK_OK
   */
  int32_t (*set_receiver_position)(
    const NeQuick2_MLF2_handle,
    const double_t longitude_degree,
    const double_t latitude_degree,
    const double_t height_meters);

  /** Sets the satellite position using WGS-84 ellipsoidal coordinates
   *
   * @param[in] NeQuick2_MLF2_handle NeQuick2-MLF2 handle
   * @param[in] longitude_degree Geodetic longitude from receiver (degrees)
   * @param[in] latitude_degree Geodetic latitude from receiver (degrees) range accepted [-90, +90]
   * @param[in] height_meters Geodetic height from receiver (m)
   *
   * @return on success NEQUICK_OK
   */
  int32_t (*set_satellite_position)(
    const NeQuick2_MLF2_handle,
    const double_t longitude_degree,
    const double_t latitude_degree,
    const double_t height_meters);

  /** Sets the Machine Learning models input parameters
   *
   * Two Machine Learning models are used to calculate both parameters f0F2 and hmF2.
   *
   * @param[in] hm_model ML model for hmF2 computation
   * @param[in] fo_model ML model for f0F2 computation
   * @param[in] year Current year of the simulation
   * @param[in] month Current month of the simulation [1, 12]
   * @param[in] day Current day of the simulation [1, 31]
   * @param[in] ut UT time (hours) range accepted [0, 24]
   * @param[in] ap Daily equivalent planetary amplitude
   * @param[in] kp 3-hour Planetary Kp-index
   * @param[in] smoothed_f107_365 Smoothed F10.7 value on 365 days around the current date of
   *   the simulation. (The mean on 182 days before, the curretn day, and 182 days after)
   *
   * @return on success NEQUICK_OK
   */
  int32_t (*set_F2_ML_layer_data)(
    void* hm_model,
    void* fo_model,
    uint16_t year,
    uint8_t month,
    uint8_t day,
    double_t ut,
    double_t ap,
    double_t kp,
    double_t smoothed_f107_365);

  /** Gets the receiver MODIP.
   * Needs a previous call to set_receiver_position.
   *
   * @param[in] NeQuick2_MLF2_handle NeQuick2-MLF2 handle
   *
   * @return MODIP in degrees
   */
  double_t (*get_modip)(const NeQuick2_MLF2_handle);

  /** Gets the Slant Total Electron Content in TECU.
   *  1 TECU equals 10<SUP>16</SUP> electrons/m<SUP>2</SUP>
   *
   * for each satellite-receiver link<br>
   *  Obtain receiver MODIP<SUB>U</SUB> using &phi;<SUB>i</SUB>, lambda;<SUB>i</SUB><br>
   *  Call NeQuick2-MLF2 STEC integration routine (#NeQuick2_MLF2_integrate) for path (x,y,z)<SUB>i</SUB> to (x,y,z)<SUB>j</SUB>
   *
   * @param[in] NeQuick2_MLF2_handle NeQuick2-MLF2 handle
   * @param[out] TEC ouput parameter total electron content in TECU (10<SUP>16</SUP> electrons/m<SUP>2</SUP>)
   *
   * @return on success NEQUICK_OK
   */
  int32_t (*get_total_electron_content)(
    const NeQuick2_MLF2_handle,
    double_t* const TEC);

  /** Input data set to std output
   *
   * @param[in] NeQuick2_MLF2_chandle NeQuick2-MLF2 handle
   */
  void (*input_data_to_std_output)(NeQuick2_MLF2_chandle);

  /** Input data set to file
   *
   * @param[in] NeQuick2_MLF2_chandle NeQuick2-MLF2 handle
   * @param[in] pFile pointer to a valid FILE structure
   * @param[in] pFormat the fprintf format
   */
  void (*input_data_to_output)(
    NeQuick2_MLF2_chandle,
    FILE* const pFile,
    const char* const pFormat);
};

/** NeQuick2-MLF2 library interface. */
extern const struct NeQuick2_MLF2_library NeQuick2MLF2;

#endif // NEQUICK2_MLF2_H
