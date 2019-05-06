INCLUDE(${WiFiTool_SOURCE_DIR}/config/WiFiConfig.cmake)

aux_source_directory(. NETFILTER_SRC_LIST)

ADD_LIBRARY(netfilter SHARED ${NETFILTER_SRC_LIST})

set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}") 

TARGET_LINK_LIBRARIES(netfilter -lnl-genl) 
TARGET_LINK_LIBRARIES(netfilter -llog) 
TARGET_LINK_LIBRARIES(netfilter -lradiotap) 
TARGET_LINK_LIBRARIES(netfilter -lairdump) 


