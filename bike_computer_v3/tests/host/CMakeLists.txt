

if(PICO_PLATFORM MATCHES "host")
  message("[INFO] [TEST] generate host tests")
  add_executable(tests_host host_main.cpp)
  target_link_libraries(tests_host bcTest) #pico_host_sdl

  add_test( NAME test_on_host COMMAND $<TARGET_FILE:tests_host> )


  add_executable(test_parser test_parser.cpp test_parser_data.cpp)
  target_link_libraries(test_parser bcTest Parser)

  add_test( NAME test_parser COMMAND $<TARGET_FILE:test_parser> )

endif()