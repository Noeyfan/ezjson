cmake_minimum_required (VERSION 2.6)
project (ez_json CXX)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

add_library(ezjson ezjson.cc)
add_executable(ezjson_test ../tst/test.cc)
target_link_libraries(ezjson_test ezjson)
