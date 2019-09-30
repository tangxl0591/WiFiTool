INCLUDE(${WiFiTool_SOURCE_DIR}/config/WiFiConfig.cmake)

aux_source_directory(. NETFILTER_SRC_LIST)

ADD_LIBRARY(netfilter SHARED ${NETFILTER_SRC_LIST})

set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}") 

TARGET_LINK_LIBRARIES(netfilter log)
TARGET_LINK_LIBRARIES(netfilter netcore)
TARGET_LINK_LIBRARIES(netfilter filtercore)
TARGET_LINK_LIBRARIES(netfilter airdump)
TARGET_LINK_LIBRARIES(netfilter radiotap)
TARGET_LINK_LIBRARIES(netfilter utils)

