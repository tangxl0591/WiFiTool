INCLUDE(${WiFiTool_SOURCE_DIR}/config/WiFiConfig.cmake)

SET(MAIN_SRC_LIST ./main.c ) 
ADD_EXECUTABLE(main ${MAIN_SRC_LIST})

TARGET_LINK_LIBRARIES(main -lusb-1.0) 
TARGET_LINK_LIBRARIES(main -llog) 
TARGET_LINK_LIBRARIES(main -lWifiDriver) 

set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}") 