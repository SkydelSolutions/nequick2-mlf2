/**
 * NeQuick2-MLF2 public interface implementation
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
 *
 */
#include "NeQuick2_MLF2.h"

#include <assert.h>
#include <malloc.h>

#include "NeQuick2_MLF2_context.h"
#include "NeQuickG_JRC_error.h"
#include "NeQuick2_MLF2_TEC_integration.h"
#include "NeQuickG_JRC_ray.h"
#include "NeQuick2_MLF2_iono_F2_layer_ml.h"

/** Sets the modip in the context using the station position
 *
 * input data is converted to km if necessary
 *
 * @param[in, out] pContext NeQuick2-MLF2 context
 */
static void get_modip_impl(
  NeQuick2_MLF2_context_t* const pContext) {
  input_data_to_km(&pContext->input_data);
  NeQuick2_MLF2_modip_get(&pContext->modip, &pContext->input_data.station_position);
}

/** Returns the modip in degrees
 * @see {@link get_modip_impl}
 * @param[in] handle NeQuick2-MLF2 handle
 * @return modip in degrees
 */
static double_t get_modip_interface(NeQuick2_MLF2_handle handle) {
  NeQuick2_MLF2_context_t* pContext = (NeQuick2_MLF2_context_t*)(handle);
  get_modip_impl(pContext);
  return pContext->modip.modip_degree;
}

/** Checks if the handle is valid
 * @param[in] handle NeQuick2-MLF2 handle
 * @return on success NEQUICK_OK
 */
static int32_t check_handle(NeQuick2_MLF2_chandle handle) {
  if (handle == NEQUICK_2_MLF2_INVALID_HANDLE) {
    NEQUICK_ERROR_RETURN(
      NEQUICK_ERROR_SRC_INPUT_DATA,
      NEQUICK_HANDLE_NULL,
      "NULL handle");
  }
  return NEQUICK_OK;
}

/** {@ref NeQuick2_MLF2_library.init} */
static int32_t init(
  NeQuick2_MLF2_handle* const pHandle) {

  *pHandle = NEQUICK_2_MLF2_INVALID_HANDLE;

  *pHandle = malloc(sizeof(NeQuick2_MLF2_context_t));
  if (*pHandle == NEQUICK_2_MLF2_INVALID_HANDLE) {
    NEQUICK_ERROR_RETURN(
      NEQUICK_ERROR_SRC_MEMORY,
      NEQUICK_ERROR_CODE_NO_RESOURCES,
      "Not enough resources to allocate NeQuick context on the heap");
  }

  NeQuick2_MLF2_context_t* pContext = (NeQuick2_MLF2_context_t*)(*pHandle);
  assert(pContext);

  int32_t ret;

  ret = NeQuick2_MLF2_iono_profile_init(&pContext->profile);
  if (ret != NEQUICK_OK) {
    free(*pHandle);
    *pHandle = NEQUICK_2_MLF2_INVALID_HANDLE;
    return ret;
  }

  return NEQUICK_OK;
}

/** {@ref NeQuick2_MLF2_library.close} */
static void close(NeQuick2_MLF2_handle handle) {
  if (handle != NEQUICK_2_MLF2_INVALID_HANDLE) {

    if (NeQuick2_MLF2_is_loaded_MODIP_grid_valid())
        NeQuick2_MLF2_clear_loaded_MODIP_grid();

    NeQuick2_MLF2_F2_ml_layer_close();

    free(handle);
  }
}

/** {@ref NeQuick2_MLF2_library.set_solar_flux} */
static int32_t set_solar_activity_flux(
  const NeQuick2_MLF2_handle handle,
  double_t flux) {

  int32_t ret = check_handle(handle);
  if (ret != NEQUICK_OK) {
    return ret;
  }

  return NeQuick2_MLF2_solar_activity_set(
    &((NeQuick2_MLF2_context_t*)handle)->solar_activity,
    flux);
}

/** {@ref NeQuick2_MLF2_library.get_total_electron_content} */
static int32_t get_total_electron_content(
  const NeQuick2_MLF2_handle handle,
  double_t* const pTotal_electron_content) {

  *pTotal_electron_content = 0.0;

  int32_t ret = check_handle(handle);
  if (ret != NEQUICK_OK) {
    return ret;
  }

  NeQuick2_MLF2_context_t* pContext = (NeQuick2_MLF2_context_t*)(handle);

  input_data_to_km(&pContext->input_data);
  ret = ray_get(
    &pContext->ray,
    &pContext->input_data.station_position,
    &pContext->input_data.satellite_position);
  if (ret != NEQUICK_OK) {
    return ret;
  }

  ret = NeQuick2_MLF2_integrate(pContext, pTotal_electron_content);
  if (ret == NEQUICK_OK) {
    // Eq. 151, Eq. 202 for the correction factor
    *pTotal_electron_content = (*pTotal_electron_content / 1.0E13);
  }
  return ret;
}

/** {@ref NeQuick2_MLF2_library.set_time} */
static int32_t set_time(
  const NeQuick2_MLF2_handle handle,
  const uint8_t month,
  const double_t UTC,
  const uint16_t doy) {

  int32_t ret = check_handle(handle);
  if (ret != NEQUICK_OK) {
    return ret;
  }

  NeQuick2_MLF2_context_t* pContext = (NeQuick2_MLF2_context_t*)(handle);
  ret = NeQuick2_MLF2_time_set(&pContext->input_data.time, month, UTC, doy);

  pContext->profile.E.is_solar_declination_valid = false;

  return ret;
}

/** {@ref NeQuick2_MLF2_library.set_station_position} */
static int32_t set_station_position(
  const NeQuick2_MLF2_handle handle,
  const double_t longitude_degree,
  const double_t latitude_degree,
  const double_t height_meters) {

  int32_t ret = check_handle(handle);
  if (ret != NEQUICK_OK) {
    return ret;
  }

  NeQuick2_MLF2_context_t* pContext = (NeQuick2_MLF2_context_t*)(handle);
  return position_set(
    &pContext->input_data.station_position,
    longitude_degree,
    latitude_degree,
    height_meters,
    NEQUICK_G_JRC_HEIGHT_UNITS_METERS);
}

/** {@ref NeQuick2_MLF2_library.set_satellite_position} */
static int32_t set_satellite_position(
  const NeQuick2_MLF2_handle handle,
  const double_t longitude_degree,
  const double_t latitude_degree,
  const double_t height_meters) {

  int32_t ret = check_handle(handle);
  if (ret != NEQUICK_OK) {
    return ret;
  }

  NeQuick2_MLF2_context_t* pContext = (NeQuick2_MLF2_context_t*)(handle);
  return position_set(
    &pContext->input_data.satellite_position,
    longitude_degree,
    latitude_degree,
    height_meters,
    NEQUICK_G_JRC_HEIGHT_UNITS_METERS);
}

/** {@ref NeQuick2_MLF2_library.input_data_to_std_output} */
static void input_data_to_std_output_impl(NeQuick2_MLF2_chandle handle) {
  if (handle == NEQUICK_2_MLF2_INVALID_HANDLE) {
    return;
  }

  input_data_t input_data =
    ((const NeQuick2_MLF2_context_t* const)handle)->input_data;
  input_data_to_meters(&input_data);
  bool add_new_line = false; // to satisfy lint
  input_data_to_std_output(&input_data, add_new_line);
}

/** {@ref NeQuick2_MLF2_library.input_data_to_output} */
static void input_data_to_output_impl(
  NeQuick2_MLF2_chandle handle,
  FILE* const pFile,
  const char* const pFormat) {

  if (handle == NEQUICK_2_MLF2_INVALID_HANDLE) {
    return;
  }

  input_data_t input_data =
    ((const NeQuick2_MLF2_context_t* const)handle)->input_data;
  input_data_to_meters(&input_data);
  input_data_to_output(&input_data, pFile, pFormat);
}

const struct NeQuick2_MLF2_library NeQuick2MLF2 = {
  //lint -e{123} macro with arguments is intended
  .init = init,
  .close = close,
  .load_MODIP_grid = NeQuick2_MLF2_load_MODIP_grid,
  .set_F2_ML_layer_data = NeQuick2_MLF2_set_F2_ml_layer_data,
  .set_solar_activity_flux = set_solar_activity_flux,
  .set_time = set_time,
  .set_receiver_position = set_station_position,
  .set_satellite_position = set_satellite_position,
  .get_modip = get_modip_interface,
  .get_total_electron_content = get_total_electron_content,
  .input_data_to_std_output = input_data_to_std_output_impl,
  .input_data_to_output = input_data_to_output_impl,
};
