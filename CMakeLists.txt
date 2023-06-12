cmake_minimum_required(VERSION 3.20)
project(drider VERSION 1.0.0 DESCRIPTION "Dispatch rider lib")

if(NOT DEFINED LINK_LIBS)
message(WARNING "Using domestic log functions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_DOMESTIC_LOG")
else()
message(WARNING "Using spdlog functions")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O3 -Wextra -Wno-unused-parameter -g")

## message-generator
add_custom_target(message-headers DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/libdrider/include/register-message.h)
## libdrider
include_directories(./libdrider/include)
include_directories(./include)
add_library(drider SHARED ./libdrider/src/drider-main.cpp ./libdrider/src/drider-agent.cpp ./libdrider/src/drider-publisher.cpp ./libdrider/src/drider-subscriber.cpp)
add_dependencies(drider message-headers)
target_link_libraries(drider PUBLIC ${LINK_LIBS})
## drider-broker
include_directories(./include)
add_executable(drider-broker ./src/main.cpp ./src/register-handler.cpp ./src/drider-topic.cpp ./src/drider-subscriber-internal.cpp ./src/drider-publisher-internal.cpp)
add_dependencies(drider-broker message-headers)
target_link_libraries(drider-broker PUBLIC ${LINK_LIBS} drider)