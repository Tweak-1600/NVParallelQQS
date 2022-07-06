cmake_minimum_required(VERSION 3.10)
project(NVirsual VERSION 0.1.002) 
file(GLOB_RECURSE SRC_DIR_LIST "src/*.cxx")
	add_library(NVirsual_static STATIC "${SRC_DIR_LIST}")
target_include_directories(NVirsual_static PUBLIC "include" )
	add_library(NVirsual SHARED "${SRC_DIR_LIST}")
target_include_directories(NVirsual PUBLIC "include" )

