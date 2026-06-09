file(GLOB_RECURSE NEQUICK2_MLF2_SRC *.cpp *.c)

add_library(nequick2_mlf2 STATIC ${NEQUICK2_MLF2_SRC})

target_compile_definitions(nequick2_mlf2 PUBLIC NEQUICK_OK=0)

target_include_directories(
  nequick2_mlf2
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/NeQuick2_MLF2/src/lib/public
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NeQuick2_MLF2/src/lib/private)

target_link_libraries(nequick2_mlf2 onnx_wrapper)

set_target_properties(
  nequick2_mlf2 PROPERTIES PUBLIC_HEADER
                            ${CMAKE_CURRENT_SOURCE_DIR}/nequick2_mlf2_lib.h)

install(TARGETS nequick2_mlf2
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include)
