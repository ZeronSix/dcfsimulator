cmake_minimum_required(VERSION 3.5)
project(DcfSimulator)

set(CMAKE_CXX_STANDARD 14)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-recover=address -fno-omit-frame-pointer -g")

add_executable(DcfSimulator main.cpp Simulator.cpp Simulator.h WifiChannel.cpp WifiChannel.h Node.cpp Node.h Frame.cpp Frame.h)
