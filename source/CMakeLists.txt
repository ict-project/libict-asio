################################################################
cmake_minimum_required(VERSION 3.5)
################################################################
set(LIBRARY_NAME "asio")
set(LIBRARY_DESC "Wrapper for libasio")

project(libict-${LIBRARY_NAME})

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

set(ASIO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../asio/asio/include)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/info.hpp 
  PRE_BUILD COMMAND ./info.sh ${LIBRARY_NAME} info.hpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/info.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/info.cpp 
  PRE_BUILD COMMAND ./info.sh ${LIBRARY_NAME} info.cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/info.hpp ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/all.hpp 
  PRE_BUILD COMMAND ./all.sh 
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/all.sh ${CMAKE_CURRENT_SOURCE_DIR}/info.hpp 
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

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

include_directories(BEFORE SYSTEM ${OPENSSL_INCLUDE_DIR})

add_library(ict-static STATIC ${CMAKE_SOURCE_FILES})
target_link_libraries(ict-static ${OPENSSL_CRYPTO_LIBRARIES} ${OPENSSL_SSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(ict-static  PROPERTIES OUTPUT_NAME ict-${LIBRARY_NAME})
target_include_directories(ict-static PRIVATE ${ASIO_DIR})

add_library(ict-shared SHARED ${CMAKE_SOURCE_FILES})
target_link_libraries(ict-shared ${OPENSSL_CRYPTO_LIBRARIES} ${OPENSSL_SSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(ict-shared  PROPERTIES OUTPUT_NAME ict-${LIBRARY_NAME})
target_include_directories(ict-shared PRIVATE ${ASIO_DIR})

add_executable(${PROJECT_NAME}-test all.hpp test.cpp ${CMAKE_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}-test ${OPENSSL_CRYPTO_LIBRARIES} ${OPENSSL_SSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME}-test PRIVATE ${ASIO_DIR})
target_compile_definitions(${PROJECT_NAME}-test PUBLIC -DENABLE_TESTING)


if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../.git)
  find_package(Git)
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} describe
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE "GIT_VERSION"
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    message( STATUS "Git version: ${GIT_VERSION}" )
  else(GIT_FOUND)
    set(GIT_VERSION unknown)
  endif(GIT_FOUND)
else(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../.git)
 set(GIT_VERSION unknown)
endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../.git)

file(GLOB HEADER_LIST . *.hpp)

install(TARGETS ict-static ict-shared DESTINATION lib COMPONENT libraries)
install(
  FILES ${HEADER_LIST}
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
#set(CPACK_GENERATOR "TZ;TGZ;STGZ;DEB;RPM") # Linux
#set(CPACK_GENERATOR "TZ;TGZ;STGZ")         # MacOS

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "ict-project.pl")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Mariusz Ornowski <mariusz.ornowski@ict-project.pl>")
set(CPACK_RPM_PACKAGE_LICENSE "BSD 3-clause")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} - ${LIBRARY_DESC}")
set(CPACK_PACKAGE_VERSION ${GIT_VERSION})

set(CPACK_COMPONENTS_ALL libraries headers)

set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "Lib files for: ${LIBRARY_DESC}")

set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "Headers")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION  "Header files for: ${LIBRARY_DESC}")
set(CPACK_COMPONENT_HEADERS_DEPENDS libraries)

include(CPack)
################################################################
