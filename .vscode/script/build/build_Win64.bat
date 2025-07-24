@echo off
setlocal

REM ─────────────────────────────────────────────
REM 1) Active l’environnement MSVC
cd /d "%~dp0\..\..\.."
for /f "usebackq tokens=1,* delims==" %%i in (".env\path") do set %%i=%%~j
if not defined VS_VC_PATH (
  echo ❌ VS_VC_PATH non défini.
  exit /b 1
)
call "%VS_VC_PATH%"
set PATH=%NINJA_PATH%;%PATH%

REM ─────────────────────────────────────────────
REM 2) Configure & génère C++ (Release) avec vcpkg
pushd core\cpp
cmake -S . -B ..\..\build\cpp ^
  -G "Ninja" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_TOOLCHAIN_FILE=..\..\lib\cpp\vcpkg\scripts\buildsystems\vcpkg.cmake ^
  -DVCPKG_TARGET_TRIPLET=x64-windows
if errorlevel 1 exit /b 1

REM 3) Compile
cmake --build ..\..\build\cpp
if errorlevel 1 exit /b 1

REM 5) (Optionnel) Installe dans build\cpp\install
cmake --install ..\..\build\cpp --prefix ..\..\build\cpp\install
if errorlevel 1 exit /b 1

REM Retour au dossier d’origine
popd

REM ─────────────────────────────────────────────
REM 6) Compilation Rust
cd core\rust
set CARGO_TARGET_DIR=..\..\build
cargo build --release
if errorlevel 1 exit /b 1

REM ─────────────────────────────────────────────
REM 7) Organisation finale
cd ..\..\build
if not exist EchoDice mkdir EchoDice
if not exist EchoDice\Win64 mkdir EchoDice\Win64

REM 8) Déplacement des binaires Rust
if exist release\echodice.exe (
  move /Y release\echodice.exe EchoDice\Win64\
)
if exist release\libechodice.rlib (
  move /Y release\libechodice.rlib EchoDice\Win64\
)

exit /b 0
