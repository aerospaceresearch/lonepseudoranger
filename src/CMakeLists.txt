cmake_minimum_required(VERSION 2.8)

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

add_executable(lonepseudoranger Cluster.cpp Combinations.cpp GaussianMatrix.cpp
    Reader.cpp Stats.cpp
    main.cpp )
#target_link_libraries(${TARGET} ${LIBS})
#target_link_libraries(${TARGET} marblewidget)

