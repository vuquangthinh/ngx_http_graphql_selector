cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(APP_NAME GraphQLSelector)

project(${APP_NAME})
set(CMAKE_BUILD_TYPE Release)
file(GLOB SOURCES src/*.cpp)

include_directories(3rd-parties/rapidjson/include)

add_library(${APP_NAME}_lib SHARED src/ngx_http_graphql_selector.cpp)
add_executable(${APP_NAME} ${SOURCES})
