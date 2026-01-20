cmake_minimum_required(VERSION 3.23)

project(computer-club)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(computer-club)

target_sources(computer-club
	PRIVATE
		src/main.cpp
		src/club.cpp
		src/time.cpp
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

