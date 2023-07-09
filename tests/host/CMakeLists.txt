
function(add_test_executable test_name)
    # Collect all source files except the first argument (test_name)
    set(test_sources "${ARGN}")

    # Create the test executable
    add_executable(${test_name} ${test_sources})


    target_link_libraries(${test_name} bcTest Parser BMP280) # move as param ?

    # Add the test to CTest
    # add_test(NAME ${test_name} COMMAND $<TARGET_FILE:tests_host>)

    message("[INFO] [TEST] PICO_TEST_VALGRIND=${PICO_TEST_VALGRIND}")
    if(PICO_TEST_VALGRIND MATCHES "valgrind")
      # find_program(VALGRIND "valgrind")
      set( MEMORYCHECK_COMMAND_OPTIONS --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 )
      set( MEMORYCHECK_SUPPRESSIONS_FILE ${PROJECT_SOURCE_DIR}/${test_name}/valgrind_suppress.txt)
      add_test(NAME "memcheck_${test_name}" COMMAND valgrind ${MEMORYCHECK_COMMAND_OPTIONS} $<TARGET_FILE:${test_name}> )
    else()
      add_test( NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}> )
    endif()

endfunction()

if(PICO_PLATFORM MATCHES "host")
  message("[INFO] [TEST] generate host tests")
  add_subdirectory(mock)

  add_test_executable(test_host host_main.cpp)
  add_test_executable(test_parser test_parser.cpp test_parser_data.cpp)
  add_test_executable(test_bmp280 test_bmp280.cpp)

endif()