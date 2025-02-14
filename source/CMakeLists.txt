################################################################
cmake_minimum_required(VERSION 3.25)
################################################################
set(LIBRARY_NAME "asio")
set(LIBRARY_DESC "Wrapper for libasio")

project(libict-${LIBRARY_NAME})

set(CMAKE_CXX_STANDARD 17)
find_package(Threads)

include(../libict-dev-tools/libs-include.cmake)
include(../libict-dev-tools/info-include.cmake)
include(../libict-dev-tools/test-include.cmake)
include(../libict-dev-tools/git-include.cmake)

file(GLOB CMAKE_HEADER_LIST . *.hpp)

set(CMAKE_SOURCE_FILES 
  info.cpp
  service.cpp
  asio.cpp
  resolver.cpp
  connection.cpp
  connection-string.cpp
  connection-message.cpp
  connector.cpp
  timer.cpp
  lock.cpp
  broker.cpp
)

set(CMAKE_LINK_LIBS "${CMAKE_LINK_LIBS} -lcrypto")
set(CMAKE_LINK_LIBS "${CMAKE_LINK_LIBS} -lssl")
string(STRIP ${CMAKE_LINK_LIBS} CMAKE_LINK_LIBS)

include_directories(BEFORE SYSTEM ${OPENSSL_INCLUDE_DIR})
include_directories(AFTER SYSTEM ../asio/asio/include)

add_library(ict-static-${LIBRARY_NAME} STATIC ${CMAKE_SOURCE_FILES})
target_link_libraries(ict-static-${LIBRARY_NAME} ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(ict-static-${LIBRARY_NAME}  PROPERTIES OUTPUT_NAME ict-${LIBRARY_NAME})

add_library(ict-shared-${LIBRARY_NAME} SHARED ${CMAKE_SOURCE_FILES})
target_link_libraries(ict-shared-${LIBRARY_NAME} ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(ict-shared-${LIBRARY_NAME}  PROPERTIES OUTPUT_NAME ict-${LIBRARY_NAME})

add_executable(${PROJECT_NAME}-test ${CMAKE_HEADER_LIST} test.cpp)
target_link_libraries(${PROJECT_NAME}-test ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME}-test ict-static-${LIBRARY_NAME})
target_link_libraries(${PROJECT_NAME}-test ${CMAKE_LINK_LIBS})
target_compile_definitions(${PROJECT_NAME}-test PUBLIC -DENABLE_TESTING)


################################################################
install(TARGETS ict-static-${LIBRARY_NAME} ict-shared-${LIBRARY_NAME} DESTINATION lib COMPONENT libraries)
install(
  FILES ${CMAKE_HEADER_LIST}
  DESTINATION include/libict/${LIBRARY_NAME} COMPONENT headers
)
################################################################
enable_testing()

add_test(NAME ict-asio-tc1 COMMAND ${PROJECT_NAME}-test ict asio tc1)
add_test(NAME ict-resolver-tc1 COMMAND ${PROJECT_NAME}-test ict resolver tc1)
add_test(NAME ict-resolver-tc2 COMMAND ${PROJECT_NAME}-test ict resolver tc2)
add_test(NAME ict-resolver-tc3 COMMAND ${PROJECT_NAME}-test ict resolver tc3)
add_test(NAME ict-connection-tc1 COMMAND ${PROJECT_NAME}-test ict connection tc1)
add_test(NAME ict-connection_string-tc1 COMMAND ${PROJECT_NAME}-test ict connection_string tc1)
add_test(NAME ict-connection_message-tc1 COMMAND ${PROJECT_NAME}-test ict connection_message tc1)
add_test(NAME ict-connector-tc1 COMMAND ${PROJECT_NAME}-test ict connector tc1)
add_test(NAME ict-timer-tc1 COMMAND ${PROJECT_NAME}-test ict timer tc1)
add_test(NAME ict-timer-tc2 COMMAND ${PROJECT_NAME}-test ict timer tc2)
add_test(NAME ict-timer-tc3 COMMAND ${PROJECT_NAME}-test ict timer tc3)
add_test(NAME ict-timer-tc4 COMMAND ${PROJECT_NAME}-test ict timer tc4)
add_test(NAME ict-timer-tc5 COMMAND ${PROJECT_NAME}-test ict timer tc5)
add_test(NAME ict-timer-tc6 COMMAND ${PROJECT_NAME}-test ict timer tc6)
add_test(NAME ict-timer-tc7 COMMAND ${PROJECT_NAME}-test ict timer tc7)
add_test(NAME ict-timer-tc8 COMMAND ${PROJECT_NAME}-test ict timer tc8)
add_test(NAME ict-timer-tc9 COMMAND ${PROJECT_NAME}-test ict timer tc9)
add_test(NAME ict-timer-tc10 COMMAND ${PROJECT_NAME}-test ict timer tc10)
add_test(NAME ict-lock-tc1 COMMAND ${PROJECT_NAME}-test ict lock tc1)
add_test(NAME ict-broker-tc1 COMMAND ${PROJECT_NAME}-test ict broker tc1)

################################################################
include(../libict-dev-tools/cpack-include.cmake)
################################################################
