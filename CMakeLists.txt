cmake_minimum_required( VERSION 2.8 )

project( LUMBERJACK )

set( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR} ${CMAKE_MODULE_PATH} )

set (lumberjack_src
  src/mainlumberjack.cpp
  src/cuttingplane.cpp
  src/matrix.cpp
  src/utils.cpp
)

set (lumberjack_hdr
  src/cuttingplane.h
  src/matrix.h
  src/utils.h
)

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" OFF)

if (BUILD_DOC)
  if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    
    # check if Doxygen is installed
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target( doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else (DOXYGEN_FOUND)
      message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
  endif()
endif()

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: Debug Release Profile" FORCE )
endif( NOT CMAKE_BUILD_TYPE )

set( Boost_USE_STATIC_LIBS ON )
  find_package( Boost REQUIRED )
  MESSAGE( STATUS "found boost:"
  "\nINCLUDE: ${Boost_INCLUDE_DIRS}"
  "\nLIB: ${Boost_LIBRARIES}"
)

set( SAT_ROOT "/usr/local" CACHE PATH "Additional search directory for cryptominisat library" )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-unused-local-typedef" )

include_directories( ${CONCERT_INC_DIR} "${SAT_ROOT}/include" "src" ${Boost_INCLUDE_DIRS} )
link_directories( ${CONCERT_LIB_DIR} "${SAT_ROOT}/lib" )

set( CommonLibs
  m
  pthread
  ${Boost_LIBRARIES}
  dl
  "${SAT_ROOT}/lib/libcryptominisat5.dylib"
)

add_executable( lumberjack ${lumberjack_src} ${lumberjack_hdr} )
target_include_directories( lumberjack PUBLIC "${SAT_ROOT}/include" "src" ${Boost_INCLUDE_DIRS} )
target_link_libraries( lumberjack  ${CommonLibs} )
