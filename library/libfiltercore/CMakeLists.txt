INCLUDE(${WiFiTool_SOURCE_DIR}/config/WiFiConfig.cmake)

aux_source_directory(. FILTERCORE_SRC_LIST)

ADD_LIBRARY(filtercore STATIC ${FILTERCORE_SRC_LIST})

set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}") 

