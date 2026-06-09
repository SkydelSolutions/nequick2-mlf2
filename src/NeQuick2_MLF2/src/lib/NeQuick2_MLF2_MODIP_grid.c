/** Modified Dip Latitude grid.
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
#include "NeQuick2_MLF2_MODIP_grid.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "NeQuickG_JRC_error.h"
#include "NeQuickG_JRC_interpolate.h"
#include "NeQuickG_JRC_macros.h"

#include "string_stream.h"


#define NEQUICK_ERROR_CODE_MODIP_NOT_LOADED 7

#define NEQUICK_2_MLF2_MODIP_GRID_LONG_UNIQUE_COUNT \
  (360/NEQUICK_2_MLF2_MODIP_GRID_LONG_STEP_DEGREE)

/** See F.2.4.1.
 *  In the pseudocode this correction offset is present
 *  But ESA, in its public version, do not use it
 *  and adds an explanation in the code.
 *  we follow the pseudo-code.
 */
#define NEQUICK_G_JRC_MODIP_GRID_LAT_OFFSET_CORRECTION (1.0e-6)

#define NEQUICK_2_MLF2_LAT_PADDING_POINTS_BEFORE_COUNT 1
#define NEQUICK_2_MLF2_LAT_PADDING_POINTS_AFTER_COUNT 2
#define NEQUICK_2_MLF2_LAST_LAT_DATA_INDEX \
    (NEQUICK_2_MLF2_MODIP_GRID_LAT_POINTS_COUNT \
      - NEQUICK_2_MLF2_LAT_PADDING_POINTS_AFTER_COUNT - 1)

#define NEQUICK_2_MLF2_LONG_PADDING_POINTS_BEFORE_COUNT 1
#define NEQUICK_2_MLF2_LONG_PADDING_POINTS_AFTER_COUNT 2
#define NEQUICK_2_MLF2_LAST_LONG_DATA_INDEX \
    (NEQUICK_2_MLF2_MODIP_GRID_LONG_POINTS_COUNT \
      - NEQUICK_2_MLF2_LONG_PADDING_POINTS_AFTER_COUNT - 1)


/** Type for the F2 coefficients array */
typedef double_t corrected_modip_degree_t
  [NEQUICK_2_MLF2_MODIP_GRID_LAT_POINTS_COUNT]
  [NEQUICK_2_MLF2_MODIP_GRID_LONG_POINTS_COUNT];

typedef struct grid_position_st {
  uint16_t index;
  double_t offset;
} grid_position_t;

/** TBD: in the ESA version this uses a while
 * so any value is finally corrected.
 */
static void grid_long_fix_index(
  grid_position_t * const pGrid_position, int16_t index) {
  if (index < 0) {
    assert(index > NEQUICK_2_MLF2_MODIP_GRID_LONG_UNIQUE_COUNT);
    pGrid_position->index = (uint16_t)(index +
      NEQUICK_2_MLF2_MODIP_GRID_LONG_UNIQUE_COUNT);
  } else if (index >=
      NEQUICK_2_MLF2_MODIP_GRID_LONG_UNIQUE_COUNT) {
    pGrid_position->index = (uint16_t)(index -
      NEQUICK_2_MLF2_MODIP_GRID_LONG_UNIQUE_COUNT);
  } else {
    pGrid_position->index = (uint16_t)index;
  }
}

static void grid_get_long_position(
  double_t longitude_degree,
  grid_position_t * const pGrid_position) {

  double_t index_with_offset =
    (longitude_degree + NEQUICK_G_JRC_LONG_MAX_VALUE_DEGREE) /
    NEQUICK_2_MLF2_MODIP_GRID_LONG_STEP_DEGREE;

  double_t _index = floor(index_with_offset);
  pGrid_position->offset = (index_with_offset - _index);

  // and, if required, adjust for sign and wrap to grid
  grid_long_fix_index(pGrid_position, (int16_t)_index);
}

static void grid_get_lat_position(
  double_t latitude_degree,
  grid_position_t * const pGrid_position) {

  double_t index_with_offset =
    (latitude_degree + NEQUICK_G_JRC_LAT_MAX_VALUE_DEGREE) /
    NEQUICK_2_MLF2_MODIP_GRID_LAT_STEP_DEGREE;

  assert(index_with_offset > 0.0);
  double_t _index =
    floor(index_with_offset -
                   NEQUICK_G_JRC_MODIP_GRID_LAT_OFFSET_CORRECTION);
  pGrid_position->offset = (index_with_offset - _index);
  pGrid_position->index = (uint16_t)_index;
}

/**
 * Interpolate across lat grid
 */
static double_t
grid_get_lon_interpol_point(
  const grid_position_t* const pLatitude,
  uint_fast16_t longitude_grid_index) {

    const corrected_modip_degree_t* corrected_modip_degree = NULL;
    if (!NeQuick2_MLF2_is_loaded_MODIP_grid_valid()) {
      NEQUICK_ERROR_RETURN(
        NEQUICK_ERROR_SRC_F2_LAYER,
        NEQUICK_ERROR_CODE_MODIP_NOT_LOADED,
        "MODIP grid string has not been loaded");
    }
    corrected_modip_degree = &(NeQuick2_MLF2_loaded_MODIP_grid.corrected_modip_degree);


  double_t
    lat_interpol_points[NEQUICK_G_JRC_INTERPOLATE_POINT_COUNT];
  size_t lat_grid_index = pLatitude->index;
  for (size_t i = 0;
        i < NEQUICK_G_JRC_INTERPOLATE_POINT_COUNT; i++) {
    lat_interpol_points[i] = (*corrected_modip_degree)
                               [lat_grid_index++]
                               [longitude_grid_index];
  }
  return interpolation_third_order(
    lat_interpol_points,
    pLatitude->offset);
}

static double_t grid_2D_interpolation(
  grid_position_t* pLongitude,
  const grid_position_t* const pLatitude) {

  double_t
    lon_interpol_points[NEQUICK_G_JRC_INTERPOLATE_POINT_COUNT];

  for (size_t i = 0;
        i < NEQUICK_G_JRC_INTERPOLATE_POINT_COUNT; i++) {
    lon_interpol_points[i] = grid_get_lon_interpol_point(
                              pLatitude, pLongitude->index++);
  }
  return interpolation_third_order(
    lon_interpol_points,
    pLongitude->offset);
}

void NeQuick2_MLF2_modip_grid_interpolate(
  const position_t* const pPosition,
  double_t *pModip_degree) {

  *pModip_degree = 0;

  // Obtain grid <index, offset> for longitude
  grid_position_t longitude;
  grid_get_long_position(pPosition->longitude.degree, &longitude);

  // Obtain grid <index, offset> for latitude
  grid_position_t latitude;
  grid_get_lat_position(pPosition->latitude.degree, &latitude);

  *pModip_degree = grid_2D_interpolation(&longitude, &latitude);
}


NeQuick2_MLF2_modip_grid_t NeQuick2_MLF2_loaded_MODIP_grid = {0};

int32_t NeQuick2_MLF2_load_MODIP_grid(const char *data) {

  if (!data) {
    NEQUICK_ERROR_RETURN(
      NEQUICK_ERROR_SRC_F2_LAYER,
      NEQUICK_ERROR_CODE_BAD_MODIP_GRID_FILE,
      "MODIP grid string is NULL");
  }

  string_stream_t stream;
  string_stream_init(&stream, data);

  NeQuick2_MLF2_modip_grid_t *pGrid = &NeQuick2_MLF2_loaded_MODIP_grid;

  size_t latitude_index;
  size_t longitude_index;
  for (latitude_index = NEQUICK_2_MLF2_LAT_PADDING_POINTS_BEFORE_COUNT;
     latitude_index < NEQUICK_2_MLF2_MODIP_GRID_LAT_POINTS_COUNT
                        - NEQUICK_2_MLF2_LAT_PADDING_POINTS_AFTER_COUNT;
     latitude_index++) {
    for (longitude_index = NEQUICK_2_MLF2_LONG_PADDING_POINTS_BEFORE_COUNT;
       longitude_index < NEQUICK_2_MLF2_MODIP_GRID_LONG_POINTS_COUNT
                           - NEQUICK_2_MLF2_LONG_PADDING_POINTS_AFTER_COUNT;
       longitude_index++) {
      const char *element = string_stream_curr_element(&stream);
      if (!element || sscanf(element, "%lf",
                             &(pGrid->corrected_modip_degree
                                 [latitude_index]
                                 [longitude_index])) != 1) {
        NEQUICK_ERROR_RETURN(
          NEQUICK_ERROR_SRC_MODIP,
          NEQUICK_ERROR_CODE_BAD_MODIP_STRUCTURE,
          "error scanning modip grid using sscanf");
      }
      string_stream_move_to_next_element(&stream);
    }
  }

  // Handle longitude wrap-around
  for (latitude_index = NEQUICK_2_MLF2_LAT_PADDING_POINTS_BEFORE_COUNT;
      latitude_index < NEQUICK_2_MLF2_MODIP_GRID_LAT_POINTS_COUNT
                         - NEQUICK_2_MLF2_LAT_PADDING_POINTS_AFTER_COUNT;
      latitude_index++) {
      // Last column to first padding
      pGrid->corrected_modip_degree[latitude_index][0] =
        pGrid->corrected_modip_degree
          [latitude_index]
          [NEQUICK_2_MLF2_LAST_LONG_DATA_INDEX];

      // First 2 columns to last padding
      pGrid->corrected_modip_degree
        [latitude_index]
        [NEQUICK_2_MLF2_LAST_LONG_DATA_INDEX + 1] =
        pGrid->corrected_modip_degree
          [latitude_index]
          [NEQUICK_2_MLF2_LONG_PADDING_POINTS_BEFORE_COUNT];
      pGrid->corrected_modip_degree
        [latitude_index]
        [NEQUICK_2_MLF2_LAST_LONG_DATA_INDEX + 2] =
        pGrid->corrected_modip_degree
          [latitude_index]
          [NEQUICK_2_MLF2_LONG_PADDING_POINTS_BEFORE_COUNT + 1];
  }

  // Handle latitude padding (poles)
  for (longitude_index = NEQUICK_2_MLF2_LONG_PADDING_POINTS_BEFORE_COUNT;
      longitude_index < NEQUICK_2_MLF2_MODIP_GRID_LONG_POINTS_COUNT
                          - NEQUICK_2_MLF2_LONG_PADDING_POINTS_AFTER_COUNT;
      longitude_index++) {
      // Second column to first padding
      pGrid->corrected_modip_degree[0][longitude_index] =
        pGrid->corrected_modip_degree
          [NEQUICK_2_MLF2_LAT_PADDING_POINTS_BEFORE_COUNT + 1]
          [longitude_index];

      // Penultimate 2 lines to the end padding
      pGrid->corrected_modip_degree
          [NEQUICK_2_MLF2_LAST_LAT_DATA_INDEX + 1]
          [longitude_index] =
        pGrid->corrected_modip_degree
          [NEQUICK_2_MLF2_LAST_LAT_DATA_INDEX - 1]
          [longitude_index];
      pGrid->corrected_modip_degree
          [NEQUICK_2_MLF2_LAST_LAT_DATA_INDEX + 2]
          [longitude_index] =
        pGrid->corrected_modip_degree
          [NEQUICK_2_MLF2_LAST_LAT_DATA_INDEX - 1]
          [longitude_index];
  }

  pGrid->is_loaded = true;
  return NEQUICK_OK;
}

void NeQuick2_MLF2_clear_loaded_MODIP_grid() {

    NeQuick2_MLF2_loaded_MODIP_grid.is_loaded = false;
}

bool NeQuick2_MLF2_is_loaded_MODIP_grid_valid() {

  return NeQuick2_MLF2_loaded_MODIP_grid.is_loaded;
}

#undef NEQUICK_G_JRC_MODIP_GRID_LAT_OFFSET_CORRECTION

#undef NEQUICK_2_MLF2_LAT_PADDING_POINTS_BEFORE_COUNT
#undef NEQUICK_2_MLF2_LAT_PADDING_POINTS_AFTER_COUNT
#undef NEQUICK_2_MLF2_LAST_LAT_DATA_INDEX

#undef NEQUICK_2_MLF2_LONG_PADDING_POINTS_BEFORE_COUNT
#undef NEQUICK_2_MLF2_LONG_PADDING_POINTS_AFTER_COUNT
#undef NEQUICK_2_MLF2_LAST_LONG_DATA_INDEX

#undef NEQUICK_G_JRC_MODIP_GRID_LONG_UNIQUE_COUNT
