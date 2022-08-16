cmake_minimum_required(VERSION 3.20)
project(drider VERSION 1.0.0 DESCRIPTION "Dispatch rider lib")


## libdrider
include_directories(./libdrider/include)
add_executable(message-generator ./libdrider/src/message-generator.cpp)

execute_process (
    COMMAND bash -c "release/bin/message-generator drider/drider-devel/message-definitions drider/drider-devel/libdrider/include"
    message(WARNING "Executed message generation")
)

add_library(drider SHARED ./libdrider/src/drider-main.cpp ./libdrider/src/drider-agent.cpp ./libdrider/src/drider-publisher.cpp ./libdrider/src/drider-subscriber.cpp)
target_link_libraries(drider PUBLIC ${LINK_LIBS})
## drider-broker
include_directories(./include)
add_executable(drider-broker ./src/main.cpp ./src/register-handler.cpp ./src/drider-topic.cpp ./src/drider-subscriber-internal.cpp ./src/drider-publisher-internal.cpp)
target_link_libraries(drider-broker PUBLIC ${LINK_LIBS} drider)

