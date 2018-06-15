# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(CMTScan)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build CMTScan with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#CRY_DIR
SET(CRY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../cry_v1.7)
SET(CRY_INCLUDE_DIRS ${CRY_DIR}/src)
SET(CRY_LIBRARIES ${CRY_DIR}/lib/libCRY.a)

INCLUDE_DIRECTORIES( ${CRY_INCLUDE_DIRS} )
include_directories(/home/vallois/rapidjson/include/rapidjson)
#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)


#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(CMTScan CMTScan.cc ${sources} ${headers})
##target_link_libraries(CMTScan ${Geant4_LIBRARIES})
target_link_libraries(CMTScan ${Geant4_LIBRARIES} ${CRY_LIBRARIES})
#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build CMTScan. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(EXAMPLECMTScan_SCRIPTS
  init_vis.mac
  run1.mac
  vis.mac
  )

foreach(_script ${EXAMPLECMTScan_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
   COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS CMTScan DESTINATION bin)


