#RootUtils.cmake
# Created on: 16 May 2017
#     Author: Nicola Mori

# Extracted from FindROOT.cmake
#
# function(CREATE_LINKDEF linkdef
#                         CLASS class1 class2 ... 
#                         NOSTREAMER class1, class2 ...
#                         DEPENDS depend1 depend2 ...
#                         INSTALL installfolder)
function(root_create_linkdef linkdef)
  cmake_parse_arguments(ARG "" "INSTALL" "CLASS;ENUM;NOSTREAMER;DEPENDS" ${ARGN})
  
  set(FILENAME ${CMAKE_CURRENT_BINARY_DIR}/${linkdef}.h)
  
  set(generate 0)
  
  #---Check existence of linkdef file------------------
  if(NOT EXISTS ${FILENAME})
    set(generate 1)
  endif()

  #---Check dependencies------------------
  if(${generate} EQUAL 0)
    foreach(depend ${ARG_DEPENDS})
      if(${depend} IS_NEWER_THAN ${FILENAME})
        set(generate 1)
      endif()
    endforeach()
  endif()
  
  if(generate)
    #---Generate linkdef------------------
    foreach(class ${ARG_CLASS})
      set(classes "${classes} ${class}")
    endforeach()
    foreach(enum ${ARG_ENUM})
      set(enums "${enums} ${enum}")
    endforeach()
    message(STATUS "Generating LinkDef file ${linkdef}.h for classes: " ${classes} " and enums: " ${enums})
  
    file(WRITE ${FILENAME} "#ifdef __CINT__\n")
    file(APPEND ${FILENAME} "#pragma link off all globals;\n")
    file(APPEND ${FILENAME} "#pragma link off all classes;\n")
    file(APPEND ${FILENAME} "#pragma link off all functions;\n")
    file(APPEND ${FILENAME} "#pragma link C++ nestedclasses;\n")
    file(APPEND ${FILENAME} "#pragma link C++ nestedtypedefs;\n")
    foreach(class ${ARG_CLASS})
      SET(STREAMERSUFFIX "+")
      foreach(nsClass ${ARG_NOSTREAMER})
        if (class STREQUAL nsClass)
          set(STREAMERSUFFIX "-")
        endif()
      endforeach()
      file(APPEND ${FILENAME} "#pragma link C++ class ${class}${STREAMERSUFFIX};\n")
    endforeach()
    foreach(enum ${ARG_ENUM})
      file(APPEND ${FILENAME} "#pragma link C++ enum ${enum};\n")
    endforeach()
    file(APPEND ${FILENAME} "#endif\n")
  endif()

  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  set(${linkdef} ${FILENAME} PARENT_SCOPE)

  if(DEFINED ARG_INSTALL)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${linkdef}.h DESTINATION ${ARG_INSTALL})
  endif()

endfunction()