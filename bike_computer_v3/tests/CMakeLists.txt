

add_subdirectory(bc_test)

if(PICO_PLATFORM MATCHES "rp2040")
add_subdirectory(target)
# message("generate target tests")
#   add_subdirectory(target/tc)

#     message("PICO_TEST_FILE=${PICO_TEST_FILE}")
#     if( NOT PICO_TEST_FILE )
#             SET(PICO_TEST_FILE pico_test)
#             message("SET PICO_TEST_FILE = ${PICO_TEST_FILE}")
#     endif()
#   add_executable(${PICO_TEST_FILE} target/target_main.cpp)
#   target_link_libraries(${PICO_TEST_FILE} bcTest test_main)

#   pico_enable_stdio_usb(${PICO_TEST_FILE} ENABLED)
#   pico_enable_stdio_uart(${PICO_TEST_FILE} 0)
#   pico_add_extra_outputs(${PICO_TEST_FILE})




# PICO_PLATFORM=host

#test_main


else()
  add_subdirectory(host)
  # message("generate host tests")
  # # add_subdirectory(host/pico_hw_mock)
  # add_executable(tests_host host/host_main.c)
  # target_link_libraries(tests_host bcTest ) #pico_host_sdl

  # add_test(
  # NAME
  # test_on_host
  # COMMAND
  # $<TARGET_FILE:tests_host>
  # )


endif()