cmake_minimum_required(VERSION 3.10)
project(analyse)

set(CMAKE_CXX_STANDARD 11)

include_directories( ./include )
include_directories(/home/vallois/rapidjson/include/rapidjson)

FIND_PACKAGE( ROOT REQUIRED )
INCLUDE_DIRECTORIES( SYSTEM ${ROOT_INCLUDE_DIRS} )
LINK_LIBRARIES( ${ROOT_LIBRARIES} )
ADD_DEFINITIONS( ${ROOT_DEFINITIONS} )

add_executable(analyse main.cpp src/caloHit.cc include/caloHit.h src/Cluster.cc include/Cluster.h src/analyse.cc include/analyse.h src/trace.cc include/trace.h src/rootreader.cc include/rootreader.h)