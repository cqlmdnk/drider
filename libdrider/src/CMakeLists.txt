
include_directories(../include)
link_directories(${h1ve_SOURCE_DIR}/libs/public/spdlog/src)
include_directories(${h1ve_SOURCE_DIR}/libs/public/spdlog/include)

add_library(drider SHARED drider-main.cpp drider-agent.cpp drider-publisher.cpp drider-subscriber.cpp register-message.cpp)

target_link_libraries(spdlog)