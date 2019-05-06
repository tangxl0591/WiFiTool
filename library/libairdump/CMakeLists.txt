INCLUDE(${WiFiTool_SOURCE_DIR}/config/WiFiConfig.cmake)

aux_source_directory(. AIRDUMP_SRC_LIST)

ADD_LIBRARY(airdump STATIC ${AIRDUMP_SRC_LIST})

set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}") 

TARGET_LINK_LIBRARIES(airdump -llog) 

