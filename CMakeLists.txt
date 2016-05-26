cmake_minimum_required(VERSION 3.0)

include_directories(include)

add_executable(builder-example example.cpp main.cpp)

target_compile_options(builder-example PRIVATE
    -std=c++1z
	-Weverything
	-Wno-c++98-compat
	-Wno-c++98-compat-pedantic
	-Wno-padded
	-Wno-shadow
	-Wno-weak-vtables
	-pedantic
	-pedantic-errors)
