

add_subdirectory(bc_test)

set(PICO_TEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/hw/tests")
set(PICO_TEST_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/pico_tests")
# message("------------------------------- add sub pico test --------------------------------")
add_subdirectory(external/pico_tests/pico_tests/)
# message("------------------------------- picotest end --------------------------------")

if(PICO_PLATFORM MATCHES "rp2040")

add_subdirectory(hw)

else()
  add_subdirectory(host)
endif()