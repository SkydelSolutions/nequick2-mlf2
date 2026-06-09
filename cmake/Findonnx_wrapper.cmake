set(onnx_wrapper_ROOT ${EXTERNAL_DIRECTORY}/onnx_wrapper)

find_path(
  onnx_wrapper_INCLUDE_DIRS
  NAMES onnx_wrapper_interface.h
  HINTS ${onnx_wrapper_ROOT}/include NO_CACHE)


if(WIN32)
  string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_NAME)
  find_library(
    onnx_wrapper_LIBRARIES
    NAMES onnx_wrapper
    HINTS ${onnx_wrapper_ROOT}/lib/win64/${BUILD_TYPE_NAME} NO_CACHE)
else()
  find_library(
    onnx_wrapper_LIBRARIES
    NAMES onnx_wrapper
    HINTS ${onnx_wrapper_ROOT}/lib/linux NO_CACHE)
endif(WIN32)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  onnx_wrapper DEFAULT_MSG onnx_wrapper_INCLUDE_DIRS onnx_wrapper_LIBRARIES)

add_library(onnx_wrapper INTERFACE IMPORTED)

target_link_libraries(onnx_wrapper INTERFACE ${onnx_wrapper_LIBRARIES})
target_include_directories(onnx_wrapper SYSTEM
                           INTERFACE ${onnx_wrapper_INCLUDE_DIRS})

mark_as_advanced(onnx_wrapper_INCLUDE_DIRS)
mark_as_advanced(onnx_wrapper_LIBRARIES)
