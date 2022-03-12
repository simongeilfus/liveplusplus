cmake_minimum_required (VERSION 3.6)

#check dependencies
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/LivePP)
    message( "-- Downloading live++")
    set(LIVEPP_ARCHIVE "LPP_1_6_8.zip")
    file(DOWNLOAD "https://liveplusplus.tech/downloads/${LIVEPP_ARCHIVE}" "${CMAKE_CURRENT_SOURCE_DIR}/third_party/${LIVEPP_ARCHIVE}")
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/${LIVEPP_ARCHIVE})
        file(ARCHIVE_EXTRACT INPUT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/${LIVEPP_ARCHIVE}" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/third_party/")
        file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/${LIVEPP_ARCHIVE}")
    endif()
endif()

# set the project name
project(liveplusplus)

# add sources
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/third_party/LivePP/API)
file(GLOB SOURCES "src/liveplusplus/*.cpp" "include/liveplusplus/*.h")
add_library(liveplusplus STATIC ${SOURCES})
target_include_directories(liveplusplus PUBLIC ${INCLUDE_DIR})

# compiler and linker options
target_link_options(liveplusplus PUBLIC /FUNCTIONPADMIN /OPT:NOREF /OPT:NOICF /DEBUG:FULL)
target_compile_options(liveplusplus PUBLIC /Zi)

target_compile_features(liveplusplus PRIVATE cxx_std_17)

# set the path as compile definition
target_compile_definitions(liveplusplus PUBLIC LPP_PATH="${CMAKE_CURRENT_SOURCE_DIR}/third_party/LivePP" LPP_ENABLED)