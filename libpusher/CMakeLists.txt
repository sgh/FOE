project(libpusher)
cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0020 NEW)

## Config
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
##-fvisibility=hidden -fvisibility-inlines-hidden -flto
	set(CMAKE_CXX_FLAGS                "-Wextra -Wall -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG          "-Og -g -ggdb")
	set(CMAKE_CXX_FLAGS_MINSIZEREL     "-s -Os -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE        "-s -O2 -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-s -O2 -g")
endif()
#if (NOT BUILD_SHARED_LIBS)
#	set(BUILD_SHARED_LIBS on)
#endif()

# Find includes in corresponding build directories
# set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find Qt libraries
find_package(Qt5Core)
find_package(Qt5Network)

## Library
set(LIBOURCES   pusher.cpp)
set(LIBHEADERS  pusher.h pusher_p.h)
add_library(pusher  ${LIBOURCES} ${LIBHEADERS})

# Use the Core module from Qt 5.
target_link_libraries(pusher Qt5::Core Qt5::Network)


## Application
set(APPSOURCES   main.cpp)
link_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(main ${APPSOURCES})
target_link_libraries(main pusher)
target_link_libraries(main Qt5::Core)
add_dependencies(main pusher)
