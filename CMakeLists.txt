cmake_minimum_required(VERSION 3.15)
project(Gcalc)

set(CMAKE_CXX_STANDARD 11)

add_executable(Gcalc main.cpp Parser.cpp Parser.h Gcalc.cpp Gcalc.h Graph.cpp Graph.h GraphExceptions.h Auxiliaries.cpp Auxiliaries.h)