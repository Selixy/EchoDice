@echo off
setlocal enableextensions enabledelayedexpansion
cls

REM ─────────────────────────────────────────────
REM 1) Aller à la racine du projet
cd /d "%~dp0\..\..\.."
set "ROOT_DIR=%CD%"

REM ─────────────────────────────────────────────
REM 2) Charger MSVC & Ninja
for /f "usebackq tokens=1,* delims==" %%I in (".env\path") do set "%%I=%%~J"
if not defined VS_VC_PATH (
  echo ERREUR : VS_VC_PATH non défini.
  exit /b 1
)
call "%VS_VC_PATH%"
if not defined NINJA_PATH (
  echo ERREUR : NINJA_PATH non défini.
  exit /b 1
)
set "PATH=%NINJA_PATH%;%PATH%"

echo.
echo [1/6] Racine du projet : %ROOT_DIR%

REM ─────────────────────────────────────────────
REM 3) Préparer les dossiers de build
set "BUILD_DIR=%ROOT_DIR%\build"
set "BUILD_CPP_DIR=%BUILD_DIR%\cpp"
set "BUILD_RUST_DIR=%BUILD_DIR%\rust"
set "OUTPUT_DIR=%BUILD_DIR%\EchoDice"

if exist "%BUILD_DIR%" (
  echo [2/6] Suppression de %BUILD_DIR%\…
  rmdir /s /q "%BUILD_DIR%"
)
echo [2/6] Création de %BUILD_DIR% et %OUTPUT_DIR%\…
mkdir "%BUILD_CPP_DIR%" "%OUTPUT_DIR%"

REM ─────────────────────────────────────────────
REM 4) Configuration & compilation C++ (Release)
echo [3/6] Configuration C++…
cmake -S "%ROOT_DIR%\core\cpp" -B "%BUILD_CPP_DIR%" -G "Ninja" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_TOOLCHAIN_FILE="%ROOT_DIR%\lib\cpp\vcpkg\scripts\buildsystems\vcpkg.cmake" ^
  -DVCPKG_TARGET_TRIPLET=x64-windows
if errorlevel 1 exit /b 1

echo [4/6] Compilation C++…
cmake --build "%BUILD_CPP_DIR%" --config Release
if errorlevel 1 exit /b 1

echo Copie de la DLL C++ vers %OUTPUT_DIR%\…
copy /Y "%BUILD_CPP_DIR%\API_Network.dll" "%OUTPUT_DIR%\" >nul 2>nul

echo Copie de l’import-lib et renommage pour Rust…
copy /Y "%BUILD_CPP_DIR%\API_Network.lib" "%OUTPUT_DIR%\" >nul
copy /Y "%BUILD_CPP_DIR%\API_Network.lib" "%OUTPUT_DIR%\echodice_api.lib" >nul

echo Copie des DLL runtime vcpkg vers %OUTPUT_DIR%\…
copy /Y "%ROOT_DIR%\lib\cpp\vcpkg\installed\x64-windows\bin\*.dll" "%OUTPUT_DIR%\" 2>nul

REM ─────────────────────────────────────────────
REM 5) Compilation Rust (Release) avec flags de lien intégrés
echo [5/6] Compilation Rust…
cd /d "%ROOT_DIR%\core\rust"
set "CARGO_TARGET_DIR=%BUILD_RUST_DIR%"

REM On passe à rustc le chemin et le nom de la lib C++ :
set "RUSTFLAGS=-L native=%OUTPUT_DIR% -l static=echodice_api"

cargo build --release
if errorlevel 1 exit /b 1

REM ─────────────────────────────────────────────
echo [6/6] Packaging exe et nettoyage…
copy /Y "%BUILD_RUST_DIR%\release\echodice.exe" "%OUTPUT_DIR%\" >nul
copy /Y "%BUILD_RUST_DIR%\release\libechodice.rlib" "%OUTPUT_DIR%\" >nul

echo.
echo ============================================
echo Build & packaging terminés avec succès !
echo Artéfacts disponibles dans %OUTPUT_DIR%
echo ============================================
exit /b 0
