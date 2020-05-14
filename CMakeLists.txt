project(android-cmake-helloworld)

cmake_minimum_required(VERSION 3.12.0)
add_compile_options(-g -O0 -fPIC -funwind-tables -Wall -Wextra -DDEBUG -D_GNU_SOURCE -Wno-unused-function -Wno-unused-variable)

add_library(test SHARED test.cpp)
add_executable(HelloWorld main.cpp)
# target_link_libraries(HelloWorld test)

