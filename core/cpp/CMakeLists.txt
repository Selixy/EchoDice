cmake_minimum_required(VERSION 3.10)
project(echo_dice)

add_library(echo_dice STATIC add.cpp)
set_target_properties(echo_dice PROPERTIES POSITION_INDEPENDENT_CODE ON)
