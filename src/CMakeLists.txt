
#libs
link_directories(${h1ve_SOURCE_DIR}/drider/libdrider/src)
include_directories(${h1ve_SOURCE_DIR}/drider/libdrider/include)

#exec
include_directories(../include)
add_executable(drider-broker main.cpp register-handler.cpp)

#lib link
target_link_libraries(drider-broker PUBLIC drider)