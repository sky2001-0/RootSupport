cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(RootSupport VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(RootSupport INTERFACE)

target_include_directories(
  RootSupport INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/libs
  ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(ROOT 6.24 REQUIRED)
target_link_libraries(
  RootSupport INTERFACE
  ROOT::Core
  ROOT::Gpad
  ROOT::Hist
  ROOT::RIO
  ROOT::Tree
)
