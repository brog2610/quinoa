################################################################################
#
# \file      cmake/BuildType.cmake
# \author    J. Bakosi
# \copyright 2012-2015, Jozsef Bakosi, 2016, Los Alamos National Security, LLC.
# \brief     Set a default build type if none was specified
# \date      Thu 05 May 2016 07:27:52 PM MDT
#
################################################################################

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "CMAKE_BUILD_TYPE not specified, setting to 'Debug'")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type. Possible values: DEBUG | RELEASE | RELWITHDEBINFO | MINSIZEREL" FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
else()
  message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()
