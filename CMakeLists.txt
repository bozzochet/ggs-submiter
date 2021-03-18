#### Do not allow for in-source builds ####
# Remove any leftover from an eventual previous attempt to make an in-source build
file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/CMakeCache.txt ${CMAKE_SOURCE_DIR}/CMakeFiles)
# Check for in-source build
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   # Failure: this is an in-source build. Let's handle this:
   # When the CMake cache file is present in the source tree then the source tree will be used as the build tree even when launching cmake
   # later from an external folder, so the cache file must not be created. Note that it is not sufficient to just delete the cache file
   # at this point since it will be created at the end  of thecmake execution. Thus, create a directory named CMakeCache.txt to prevent the
   # creation of the cache file leveraging the resulting name conflict.
   #  file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/CMakeCache.txt)
  message(FATAL_ERROR "In-source builds are not allowed. Please make a separate folder for building, e.g.:\n mkdir build; cd build; cmake ../")
endif()

project(herd-vv)
cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD_REQUIRED 14)
set(CMAKE_CXX_STANDARD 14)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_MACOSX_RPATH ON)

set(herd-vv_VERSION "0.0")

if(NOT APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed ${CMAKE_SHARED_LINKER_FLAGS}")
endif()

find_package(ROOT)
if(ROOT_FOUND)
  message(STATUS "Found ROOT: ${ROOT_DIR} (found version ${ROOT_VERSION})") 
  include(RootUtils.cmake)
endif()

find_package(HerdSoftware)
if(HerdSoftware_FOUND)
  #message(STATUS "Found HerdSoftware: ${HerdSoftware_DIR} (found version ${HerdSoftware_VERSION})") 
  include(${HerdSoftware_DIR}/HerdSoftwareConfig.cmake)
endif()

find_package(GGS)

find_package(EventAnalysis)
if(EventAnalysis_FOUND)
  #message(STATUS "Found EventAnalysis: ${EventAnalysis_DIR} (found version ${EventAnalysis_VERSION})") 
  include_directories(${EventAnalysis_INCLUDE_DIRS})
endif()


if(APPLE)
  set (CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
endif()

add_subdirectory(commonalgo)


set(MY_PUBLIC_HEADERS
  commonalgo/GeomAcceptance/MCtruthProcess.h
  commonalgo/TreePersistence/TreePersistence.h
  commonalgo/GeomAcceptance/CaloGeomFidVolume.h
  commonalgo/Calo/CaloGlob.h
  commonalgo/Calo/CaloDig.h	
  commonalgo/Calo/CaloAxisProcess.h
 # commonalgo/Calo/CaloAxisInfo.h
  commonalgo/Calo/CaloTest.h
   commonalgo/Calo/CaloSkimHits.h	
  commonalgo/Histo/EneHisto.h
  commonalgo/Histo/MCGenSpectrum.h
  commonalgo/Histo/CaloClusteringHisto.h
  commonalgo/Histo/CaloDigHisto.h
  commonalgo/Histo/CaloAxisHisto.h	
  commonalgo/Histo/CaloMcHitsHisto.h
  commonalgo/Selection/CaloMcTrackLenghtSelection.h
  commonalgo/Selection/CaloMcHitsNumberSelection.h	
)

set_target_properties(CommonAlgo PROPERTIES PUBLIC_HEADER "${MY_PUBLIC_HEADERS}")

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)
INSTALL(TARGETS CommonAlgo 
        LIBRARY DESTINATION plugins
        PUBLIC_HEADER DESTINATION include
)
