cmake_minimum_required(VERSION 3.26)
project(cecs C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB CECS_SOURCES CONFIGURE_DEPENDS src/*.c)
add_executable(cecs ${CECS_SOURCES})

target_include_directories(cecs PUBLIC include)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(cecs PRIVATE -Wall -Wextra -Wpedantic)
endif()
