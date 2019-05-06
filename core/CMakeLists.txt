INCLUDE(${WiFiTool_SOURCE_DIR}/config/WiFiConfig.cmake)

aux_source_directory(. FILECORE_SRC_LIST)

ADD_LIBRARY(filecore STATIC ${FILECORE_SRC_LIST})

# TARGET_LINK_LIBRARIES(filecore -lcoredriver) 

# set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}") 

