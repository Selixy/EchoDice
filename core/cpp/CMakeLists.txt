cmake_minimum_required(VERSION 3.10)
project(echo_dice LANGUAGES CXX)

# ───────────────────────────────────────────────
# Utiliser C++17 strict sans extensions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ───────────────────────────────────────────────
# 1) Récupération automatique de tous les .cpp sous src/
file(GLOB_RECURSE SOURCES "src/*.cpp")

# ───────────────────────────────────────────────
# 2) Création de la librairie statique echo_dice
add_library(echo_dice STATIC ${SOURCES})

# 2.1) Exposer le dossier include/
target_include_directories(echo_dice PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# ───────────────────────────────────────────────
# 3) Intégration de libdatachannel via vcpkg
find_package(LibDataChannel CONFIG REQUIRED)

message(STATUS "CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "VCPKG_TARGET_TRIPLET = ${VCPKG_TARGET_TRIPLET}")

# 3.1) Définir la macro pour activer les portions WebRTC dans ton code
target_compile_definitions(echo_dice PUBLIC
    HAVE_WEBRTC
)

# 3.2) Lier la target importée
target_link_libraries(echo_dice PUBLIC
    LibDataChannel::LibDataChannel
)

# ───────────────────────────────────────────────
# 4) (Optionnel) générer du code position-indépendant
set_target_properties(echo_dice PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
