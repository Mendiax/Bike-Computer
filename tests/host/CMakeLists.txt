

if(PICO_PLATFORM MATCHES "host")
  message("[INFO] [TEST] generate host tests")
  add_executable(tests_host host_main.cpp)
  target_link_libraries(tests_host bcTest) #pico_host_sdl

  add_test( NAME test_on_host COMMAND $<TARGET_FILE:tests_host> )


  add_executable(test_parser test_parser.cpp test_parser_data.cpp)
  target_link_libraries(test_parser bcTest Parser)

message("[INFO] [TEST] PICO_TEST_VALGRIND=${PICO_TEST_VALGRIND}")
if(PICO_TEST_VALGRIND MATCHES "valgrind")
  # find_program(VALGRIND "valgrind")
  set( MEMORYCHECK_COMMAND_OPTIONS --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 )
  set( MEMORYCHECK_SUPPRESSIONS_FILE ${PROJECT_SOURCE_DIR}/valgrind_suppress.txt)
  add_test(NAME memcheck_test_parser COMMAND valgrind ${MEMORYCHECK_COMMAND_OPTIONS} $<TARGET_FILE:test_parser> )
else()
  add_test( NAME test_parser COMMAND $<TARGET_FILE:test_parser> )
endif()


endif()