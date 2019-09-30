INCLUDE(${WiFiTool_SOURCE_DIR}/config/WiFiConfig.cmake)

aux_source_directory(. RADIOTAP_SRC_LIST)

ADD_LIBRARY(radiotap STATIC ${RADIOTAP_SRC_LIST})

set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}") 

#TARGET_LINK_LIBRARIES(radiotap -llog) 

