cmake_minimum_required (VERSION 3.6)

# set the project name
project(liveplusplus)

# add sources
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/third_party/LPP/API)
file(GLOB SOURCES "src/liveplusplus/*.cpp" "include/liveplusplus/*.h")
add_library(liveplusplus STATIC ${SOURCES})
target_include_directories(liveplusplus PUBLIC ${INCLUDE_DIR})

# compiler and linker options
target_link_options(liveplusplus PUBLIC /FUNCTIONPADMIN /OPT:NOREF /OPT:NOICF /DEBUG:FULL)
target_compile_options(liveplusplus PUBLIC /Zi)

target_compile_features(liveplusplus PRIVATE cxx_std_17)

# set the path as compile definition
add_compile_definitions( LPP_PATH="${CMAKE_CURRENT_SOURCE_DIR}/third_party/LPP" )