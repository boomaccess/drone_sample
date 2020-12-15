cmake_minimum_required(VERSION 2.8.12)

set(PROJECT_NAME main)

project(${PROJECT_NAME})

if(NOT MSVC)
    add_definitions("-Wall -Wextra -Wno-unused-parameter -Wno-write-strings -Wno-unused-but-set-variable -Wno-unused-variable")
else()
    add_definitions("-WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(${PROJECT_NAME}
    ${PROJECT_NAME}.cpp
)

target_link_libraries(${PROJECT_NAME}
    MAVSDK::mavsdk
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk_mission
    MAVSDK::mavsdk_offboard
)