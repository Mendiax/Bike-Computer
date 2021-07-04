

if(PICO_PLATFORM MATCHES "host")
  message("[INFO] [TEST] generate host tests")
  # add_subdirectory(host/pico_hw_mock)
  add_executable(tests_host host_main.cpp)
  target_link_libraries(tests_host bcTest ) #pico_host_sdl

  add_test(
  NAME
  test_on_host
  COMMAND
  $<TARGET_FILE:tests_host>
  )


endif()