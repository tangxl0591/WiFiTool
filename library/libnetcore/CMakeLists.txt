INCLUDE(${WiFiTool_SOURCE_DIR}/config/WiFiConfig.cmake)

aux_source_directory(. NET_SRC_LIST)

ADD_LIBRARY(netcore STATIC ${NET_SRC_LIST})

set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}") 

TARGET_LINK_LIBRARIES(netcore log) 
TARGET_LINK_LIBRARIES(netcore radiotap) 
TARGET_LINK_LIBRARIES(netcore utils)
TARGET_LINK_LIBRARIES(netcore -lnl-genl) 

