cmake_minimum_required(VERSION 3.14)

find_package(ROOT REQUIRED)

set(LIB_NAME RootSupport)

project(${LIB_NAME} CXX)

add_library(${LIB_NAME} INTERFACE)

target_include_directories(${LIB_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(${LIB_NAME} INTERFACE
  ROOT::Core
  ROOT::Gpad
  ROOT::Hist
  ROOT::RIO
  ROOT::Tree
)
