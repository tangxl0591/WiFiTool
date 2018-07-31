######################################################
#						Platform					 #
######################################################


######################################################
#						Platform					 #
######################################################

INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/include) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/include/config) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/include/platform) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/third_party/libusb/include/libusb-1.0)
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/third_party/libpcap/include)
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/third_party/libpcap/include/pcap)
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/library/liblog) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/driver) 

######################################################
#						ENV     					 #
######################################################
SET(COMMON_CFLAGS "-D_BUILD_FOR_LINUX_ ${COMMON_CFLAGS}")

SET(LIBS_PATH ${WiFiTool_SOURCE_DIR}/output)
LINK_DIRECTORIES(${LIBS_PATH})
SET(LIBRARY_OUTPUT_PATH ${LIBS_PATH})
SET(EXECUTABLE_OUTPUT_PATH ${LIBS_PATH})

SET(CMAKE_C_FLAGS ${COMMON_CFLAGS})
SET(CMAKE_CXX_FLAGS ${COMMON_CFLAGS})

link_directories(${WiFiTool_SOURCE_DIR}/third_party/libusb/lib) 
link_directories(${WiFiTool_SOURCE_DIR}/third_party/libpcap/lib) 

