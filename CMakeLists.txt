cmake_minimum_required(VERSION 3.9)
project(DES)

set(CMAKE_CXX_STANDARD 11)

add_executable(DES did_table.h const.h main.cpp des.h hack.h)