INCLUDE(${WiFiTool_SOURCE_DIR}/config/WiFiConfig.cmake)

set(EXECUTABLE_NAME "WifiTool")
SET(MAIN_SRC_LIST ./main.c ) 
ADD_EXECUTABLE(main ${MAIN_SRC_LIST})


TARGET_LINK_LIBRARIES(main -lnl-genl) 
TARGET_LINK_LIBRARIES(main -lpthread) 
TARGET_LINK_LIBRARIES(main -lnetfilter)

set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}") 
