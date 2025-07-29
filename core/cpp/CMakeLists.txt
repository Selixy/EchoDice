cmake_minimum_required(VERSION 3.10)
project(API_Cpp LANGUAGES CXX)

# ─────────────────────────────────────────────────────────────
# 1) C++17 strict
set(CMAKE_CXX_STANDARD       17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS     OFF)

# ─────────────────────────────────────────────────────────────
# 2) Sources (re-génère si on ajoute des fichiers)
file(GLOB_RECURSE API_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# ─────────────────────────────────────────────────────────────
# 3) Crée la DLL echodice_api
add_library(API_Cpp SHARED ${API_SOURCES})

# 3.1) Export des symbols + activation WebRTC
target_compile_definitions(API_Cpp
    PRIVATE  API_Cpp_EXPORTS  # active __declspec(dllexport)
    PUBLIC   HAVE_WEBRTC      # pour les #ifdef HAVE_WEBRTC
)

# 3.2) Renomme la sortie en echodice_api (DLL et import-lib)
set_target_properties(API_Cpp PROPERTIES
    OUTPUT_NAME echodice_api
)

# 3.3) Include public
target_include_directories(API_Cpp
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
)

# ─────────────────────────────────────────────────────────────
# 4) Dépendances via vcpkg
find_package(LibDataChannel CONFIG REQUIRED)
find_package(ZLIB                  REQUIRED)
find_package(CURL                  REQUIRED)


target_link_libraries(API_Cpp
    PUBLIC
      LibDataChannel::LibDataChannel
      ZLIB::ZLIB
      CURL::libcurl
)

# ─────────────────────────────────────────────────────────────
# 5) Position-independent code (utile pour shared lib)
set_target_properties(API_Cpp PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# ─────────────────────────────────────────────────────────────
# 6) Sorties binaires / libs
#    • DLL et import-lib (.lib) dans bin/
#    • archive (.a / .lib) dans lib/
foreach(_CONFIG IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    set_target_properties(API_Cpp PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${_CONFIG} ${CMAKE_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY_${_CONFIG} ${CMAKE_BINARY_DIR}/bin
        ARCHIVE_OUTPUT_DIRECTORY_${_CONFIG} ${CMAKE_BINARY_DIR}/lib
    )
endforeach()
