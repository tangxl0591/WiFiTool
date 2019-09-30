INCLUDE(${WiFiTool_SOURCE_DIR}/config/WiFiConfig.cmake)

aux_source_directory(. UTILS_SRC_LIST)

ADD_LIBRARY(utils STATIC ${UTILS_SRC_LIST})

set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}") 

