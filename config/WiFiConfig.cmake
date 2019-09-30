######################################################
#						Platform					 #
######################################################


######################################################
#						Include 					 #
######################################################

INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/include) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/include/config) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/include/platform) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/filter) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/library/libnetcore) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/library/libfiltercore) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/library/liblog) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/library/libradiotap) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/library/libairdump) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/library/libutils) 
INCLUDE_DIRECTORIES(${WiFiTool_SOURCE_DIR}/third_party/libnl/include)

######################################################
#						ENV     					 #
######################################################
SET(COMMON_CFLAGS "-D_LINUX_ ${COMMON_CFLAGS}")
SET(COMMON_CFLAGS "-DCONFIG_LIBNL ${COMMON_CFLAGS}")

SET(LIBS_PATH ${WiFiTool_SOURCE_DIR}/output)
LINK_DIRECTORIES(${LIBS_PATH})
SET(LIBRARY_OUTPUT_PATH ${LIBS_PATH})
SET(EXECUTABLE_OUTPUT_PATH ${LIBS_PATH})

SET(CMAKE_C_FLAGS ${COMMON_CFLAGS})
SET(CMAKE_CXX_FLAGS ${COMMON_CFLAGS})

add_definitions("-Wall -g")

link_directories(${WiFiTool_SOURCE_DIR}/third_party/libnl/lib) 

