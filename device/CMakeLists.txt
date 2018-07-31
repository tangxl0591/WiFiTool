INCLUDE(${WiFiTool_SOURCE_DIR}/config/WiFiConfig.cmake)

SET(DRIVER_SRC_LIST driver.c usbcommon.c)

ADD_LIBRARY(WifiDriver SHARED ${DRIVER_SRC_LIST})

TARGET_LINK_LIBRARIES(WifiDriver -lusb-1.0) 
TARGET_LINK_LIBRARIES(WifiDriver -lpcap) 

