INCLUDE(${WiFiTool_SOURCE_DIR}/config/WiFiConfig.cmake)

SET(LOG_SRC_LIST log.c )

ADD_LIBRARY(log SHARED ${LOG_SRC_LIST})

