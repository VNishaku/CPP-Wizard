cmake_minimum_required(VERSION 3.10)
project(abx_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific settings
if(WIN32)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
endif()

# Add include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Download and include nlohmann_json
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Add executable
add_executable(abx_client abx_client.cpp)

# Link libraries
target_link_libraries(abx_client PRIVATE nlohmann_json::nlohmann_json)
if(WIN32)
    target_link_libraries(abx_client PRIVATE ws2_32)
endif() 