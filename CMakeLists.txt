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
#include(version.cmake)

if(NOT APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed ${CMAKE_SHARED_LINKER_FLAGS}")
endif()

find_package(ROOT)
if(ROOT_FOUND)
  message(STATUS "Found ROOT: ${ROOT_DIR} (found version ${ROOT_VERSION})") 
  include(RootUtils.cmake)
endif()

#set(HerdSoftware_DIR ${HERDINSTALL}/cmake)

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





#if (NOT DISABLE_EVENTANALYSIS)
#  find_package(EventAnalysis 0.0)
#endif()

#if(NOT DISABLE_GGS)
#  find_package(GGS 2.6.4)
#endif()

if(APPLE)
  set (CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
endif()

#add_subdirectory(MCgeneration)
add_subdirectory(commonalgo)

#include(install.cmake)

# Install the pre-commit hook to automatically format newly added code
#if(NOT DISABLE_AUTOFORMAT)
#  find_program(CLANGFORMAT_EXE clang-format)
#  if(CLANGFORMAT_EXE)
#	  execute_process(COMMAND ${CMAKE_SOURCE_DIR}/.autoformat/git-pre-commit-format install
#		          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#	                  OUTPUT_VARIABLE autoformat_install_stdout
#	                  ERROR_VARIABLE autoformat_install_stderr
#	                  RESULT_VARIABLE autoformat_install_result
#	                 )
#	  if(${autoformat_install_result} EQUAL 2)
#	    message(FATAL_ERROR "Cannot install the autoformat hook due to the following reason:\n    ${autoformat_install_stderr}")
#	  endif()
#	else()
#	  message(STATUS "Missing clang-format executable. Autoformat hook will not be installed.")  
#	endif()
#else()
#  execute_process(COMMAND ${CMAKE_SOURCE_DIR}/.autoformat/git-pre-commit-format uninstall
#		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#	          OUTPUT_VARIABLE autoformat_install_stdout
#	          ERROR_VARIABLE autoformat_install_stderr
#	          RESULT_VARIABLE autoformat_install_result
#	          )
#  if(${autoformat_install_result} EQUAL 2)
#    message(FATAL_ERROR "Cannot uninstall the autoformat hook due to the following reason:\n    ${autoformat_install_stderr}")
#  endif()
#endif()


set_target_properties(CommonAlgo PROPERTIES PUBLIC_HEADER "commonalgo/GeomAcceptance/MCtruthProcess.h")
set_target_properties(CommonAlgo PROPERTIES PUBLIC_HEADER "commonalgo/TreePersistence/TreePersistence.h")

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)
INSTALL(TARGETS CommonAlgo 
        LIBRARY DESTINATION plugins
        PUBLIC_HEADER DESTINATION include
)
