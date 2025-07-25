cmake_minimum_required(VERSION 3.10)
project(API_Network LANGUAGES CXX)

# ─────────────────────────────────────────────────────────────
# 1) C++17 strict
set(CMAKE_CXX_STANDARD       17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS     OFF)

# ─────────────────────────────────────────────────────────────
# 2) Sources
file(GLOB_RECURSE API_SOURCES
    src/*.cpp
)

# ─────────────────────────────────────────────────────────────
# 3) Crée la DLL API_Network
add_library(API_Network SHARED ${API_SOURCES})

# 3.1) Macros
target_compile_definitions(API_Network
    PRIVATE API_NETWORK_EXPORTS
    PUBLIC  HAVE_WEBRTC
)

# 3.2) Include public
target_include_directories(API_Network PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# ─────────────────────────────────────────────────────────────
# 4) WebRTC via vcpkg
find_package(LibDataChannel CONFIG REQUIRED)
target_link_libraries(API_Network PUBLIC LibDataChannel::LibDataChannel)

# ─────────────────────────────────────────────────────────────
# 5) Intégration de Zlib via vcpkg (wrapper vcpkg)
find_package(zlib REQUIRED)
target_link_libraries(API_Network PUBLIC ZLIB::ZLIB)

# ─────────────────────────────────────────────────────────────
# 6) Position-independent code
set_target_properties(API_Network PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
