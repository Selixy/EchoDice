@echo off
setlocal enableextensions enabledelayedexpansion
cls

rem ─────────────────────────────────────────────
rem 1) Aller à la racine du projet (2 niveaux sous .vscode\script)
cd /d "%~dp0\..\..\.."
set "ROOT_DIR=%CD%"
set "BUILD_DIR=%ROOT_DIR%\build"
set "BUILD_CPP_DIR=%BUILD_DIR%\cpp"
set "BUILD_RUST_DIR=%BUILD_DIR%\rust"
set "OUTPUT_DIR=%BUILD_DIR%\EchoDice"

echo.
echo [1/6] Project root: %ROOT_DIR%

rem ─────────────────────────────────────────────
rem 2) Charger MSVC & Ninja
for /f "usebackq tokens=1,* delims==" %%I in (".env\path") do set "%%I=%%~J"
if not defined VS_VC_PATH (
  echo ERROR: VS_VC_PATH not defined.
  exit /b 1
)
call "%VS_VC_PATH%"
if not defined NINJA_PATH (
  echo ERROR: NINJA_PATH not defined.
  exit /b 1
)
set "PATH=%NINJA_PATH%;%PATH%"

echo.
echo [2/6] Nettoyer et préparer build\cpp et build\EchoDice
if exist "%BUILD_DIR%" (
  rd /s /q "%BUILD_DIR%" || (
    echo ERROR: impossible de supprimer %BUILD_DIR%
    exit /b 1
  )
)
mkdir "%BUILD_CPP_DIR%" "%OUTPUT_DIR%" || (
  echo ERROR: impossible de créer les dossiers de build
  exit /b 1
)

echo.
echo [3/6] Configuring and building C++ (verbose)...
cmake -S "%ROOT_DIR%\core\cpp" -B "%BUILD_CPP_DIR%" -G Ninja ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_TOOLCHAIN_FILE="%ROOT_DIR%\lib\cpp\vcpkg\scripts\buildsystems\vcpkg.cmake" ^
  -DVCPKG_TARGET_TRIPLET=x64-windows 
if errorlevel 1 (
  echo.
  echo *** ERROR: CMake configuration failed ***
  exit /b 1
)

cmake --build "%BUILD_CPP_DIR%" --config Release --verbose
if errorlevel 1 (
  echo.
  echo *** ERROR: C++ build failed ***
  exit /b 1
)

echo.
echo [4/6] Copying C++ DLLs and LIBs to output
copy /Y "%BUILD_CPP_DIR%\API_Cpp.dll" "%OUTPUT_DIR%\" || echo Warning: failed to copy API_Cpp.dll
copy /Y "%BUILD_CPP_DIR%\API_Cpp.lib" "%OUTPUT_DIR%\echodice_api.lib" || echo Warning: failed to copy API_Cpp.lib
copy /Y "%ROOT_DIR%\lib\cpp\vcpkg\installed\x64-windows\bin\*.dll" "%OUTPUT_DIR%\" || echo Warning: failed to copy runtime DLLs

echo.
echo [5/6] Building Rust (verbose)...
cd /d "%ROOT_DIR%\core\rust"
set "CARGO_TARGET_DIR=%BUILD_RUST_DIR%"
set "RUSTFLAGS=-L native=%OUTPUT_DIR% -l static=echodice_api"
echo   RUSTFLAGS=%RUSTFLAGS%

cargo build --release --verbose
if errorlevel 1 (
  echo.
  echo *** ERROR: Rust build failed ***
  exit /b 1
)
cls

echo.
echo [6/6] Copying executable & rlib to output
copy /Y "%BUILD_RUST_DIR%\release\echodice.exe"     "%OUTPUT_DIR%\" || echo Warning: failed to copy echodice.exe
copy /Y "%BUILD_RUST_DIR%\release\libechodice.rlib" "%OUTPUT_DIR%\" || echo Warning: failed to copy libechodice.rlib

exit /b 0
