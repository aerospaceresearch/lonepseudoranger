cmake_minimum_required(VERSION 2.8)
set(TARGET lonepseudoranger)
project(${TARGET})

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../CMakeModules 
    ~/marble/sources/
    ~/marble/sources/src/lib/geodata/
    ~/marble/sources/src/lib/marble/
    ~/marble/sources/src/lib/geodata/data/
    )
find_package(Qt4 4.6.0 REQUIRED QtCore QtGui)
find_package(Marble REQUIRED)
include(${QT_USE_FILE})
include_directories(${MARBLE_INCLUDE_DIR})
set(LIBS ${LIBS} ${MARBLE_LIBRARIES} ${QT_LIBRARIES} /usr/local/lib/)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
add_subdirectory("src")

#Add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

#target_link_libraries(${TARGET} ${LIBS})
#target_link_libraries(${TARGET} marblewidget)

