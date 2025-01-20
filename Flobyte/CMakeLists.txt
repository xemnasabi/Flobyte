cmake_minimum_required(VERSION 3.15)
project(FlobyteLoginServer)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Vcpkg toolchain integration (adjust the path to vcpkg)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Find Boost
find_package(Boost 1.86 REQUIRED COMPONENTS system filesystem)

# Include project sources
set(SOURCES
    Flobyte.cpp
    PacketManager.cpp
)

set(HEADERS
    PacketManager.h
)

# Add executable
add_executable(FlobyteLoginServer ${SOURCES} ${HEADERS})

# Link Boost libraries
if(Boost_FOUND)
    target_include_directories(FlobyteLoginServer PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(FlobyteLoginServer PRIVATE Boost::system Boost::filesystem)
else()
    message(FATAL_ERROR "Boost not found. Please ensure Boost is installed and findable.")
endif()

# Output message about the project
message(STATUS "Project: Flobyte Login Server")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Boost Include Directory: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost Libraries: ${Boost_LIBRARIES}")