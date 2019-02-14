cmake_minimum_required(VERSION 3.2)

project(bulk VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

configure_file(version.h.in autoversion.h)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

add_executable(bulk main.cpp)
add_executable(test_bulk test_main.cpp)

set_target_properties(
  bulk
  test_bulk
  PROPERTIES
    COMPILE_OPTIONS "-std=c++17;-Wpedantic;-Wall;-Wextra;-Weffc++"
  )

set_target_properties(test_bulk PROPERTIES
  COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
  INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIR};${CMAKE_CURRENT_SOURCE_DIR}"
  )

target_link_libraries(
  test_bulk
  ${Boost_LIBRARIES}
  )

install(TARGETS bulk RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

message("-- CPACK_PACKAGE_VERSION: " ${CPACK_PACKAGE_VERSION})

set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_CONTACT "ad.beregovoy@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A bulk command processor")
set(CPACK_PACKAGE_DESCRIPTION "An utility that reads takes N as an commandline argument and executes commands from stdin in blocks of N")

set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
set(CPACK_DEBIAN_PACKAGE_SECTION "Development")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Anton Beregovoy <${CPACK_PACKAGE_CONTACT}>")

include(CPack)

enable_testing()

add_test(bulk_tests test_bulk)
