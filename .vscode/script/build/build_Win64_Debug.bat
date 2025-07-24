@echo off
setlocal

REM ─────────────────────────────────────────────
REM Activer l’environnement MSVC
cd /d "%~dp0\..\..\.."
for /f "usebackq tokens=1,* delims==" %%i in (".env/path") do set %%i=%%~j
if not defined VS_VC_PATH (
    echo Erreur : VS_VC_PATH non défini.
    exit /b 1
)
call "%VS_VC_PATH%"
set PATH=%NINJA_PATH%;%PATH%

REM ─────────────────────────────────────────────
REM CMake Configuration (mode Release)
cd core\cpp
cmake -G "Ninja" -B ../../build/cpp -DCMAKE_BUILD_TYPE=Debug

REM CMake Build
cmake --build ../../build/cpp --config Debug

REM CMake Install
cmake --install ../../build/cpp --prefix ../../build/cpp/install

REM ─────────────────────────────────────────────
REM Compilation Rust
cd ..\rust
set CARGO_TARGET_DIR=..\..\build
cargo build

REM ─────────────────────────────────────────────
REM Préparation du dossier final
cd ..\..\build
if not exist EchoDice mkdir EchoDice
if not exist EchoDice\Win64 mkdir EchoDice\Win64_Debug


REM ─────────────────────────────────────────────
REM Déplacement des binaires
if exist debug\echodice.exe (
    move /Y debug\echodice.exe EchoDice\Win64_Debug\
    if errorlevel 1 exit /b 1
)
if exist debug\libechodice.rlib (
    move /Y debug\libechodice.rlib EchoDice\Win64_Debug\
    if errorlevel 1 exit /b 1
)

echo Build termine avec succes.
exit /b 0


