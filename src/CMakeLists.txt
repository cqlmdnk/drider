

#libs
link_directories(${h1ve_SOURCE_DIR}/drider/libdrider/src)
include_directories(${h1ve_SOURCE_DIR}/drider/libdrider/include)

link_directories(${h1ve_SOURCE_DIR}/libs/public/spdlog/src)
include_directories(${h1ve_SOURCE_DIR}/libs/public/spdlog/include)


#exec
include_directories(../include)
add_executable(drider-broker main.cpp register-handler.cpp drider-topic.cpp drider-subscriber-internal.cpp drider-publisher-internal.cpp)

#lib link
target_link_libraries(drider-broker PUBLIC drider spdlog)

add_compile_definitions(BROKER)