@echo off
setlocal enableextensions enabledelayedexpansion

rem ─────────────────────────────────────────────
rem [1/6] Aller à la racine du projet
rem (à partir de .vscode\script, on remonte de 3 niveaux)
cd /d "%~dp0\..\..\.."
set "ROOT_DIR=%CD%"
set "BUILD_DIR=%ROOT_DIR%\build"
set "BUILD_CPP_DIR=%BUILD_DIR%\cpp"
set "BUILD_RUST_DIR=%BUILD_DIR%\rust"
set "OUTPUT_DIR=%BUILD_DIR%\EchoDice"

echo.
echo [Build] [1/6] Project root: %ROOT_DIR%
echo       Charger MSVC & Ninja

rem ─────────────────────────────────────────────
rem [2/6] Charger MSVC & Ninja depuis .env\path
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
echo [Build] [2/6] Nettoyer uniquement le contenu de build\EchoDice

rem Supprimer uniquement le contenu du dossier OUTPUT_DIR (pas tout build/)
if exist "%OUTPUT_DIR%" (
    echo   Suppression du contenu de %OUTPUT_DIR%...
    for %%f in ("%OUTPUT_DIR%\*") do (
        if exist "%%f" (
            del /f /q "%%f" > nul 2>&1
        )
    )
    for /d %%d in ("%OUTPUT_DIR%\*") do (
        rd /s /q "%%d" > nul 2>&1
    )
) else (
    mkdir "%OUTPUT_DIR%" || (
        echo ERROR: impossible de créer %OUTPUT_DIR%
        exit /b 1
    )
)

rem Créer build\cpp au besoin
if not exist "%BUILD_CPP_DIR%" (
    mkdir "%BUILD_CPP_DIR%" || (
        echo ERROR: impossible de créer %BUILD_CPP_DIR%
        exit /b 1
    )
)


rem ─────────────────────────────────────────────
rem [3/6] Configuration et compilation C++
echo.
echo [Build] [3/6] Configuring and building C++ (verbose)...
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

rem ─────────────────────────────────────────────
rem [4/6] Copier les DLLs et LIBs dans le dossier de sortie
echo.
echo [Build] [4/6] Copying C++ DLLs and LIBs to output
copy /Y "%BUILD_CPP_DIR%\echodice_api.dll" "%OUTPUT_DIR%\" || echo Warning: failed to copy echodice_api.dll
copy /Y "%BUILD_CPP_DIR%\echodice_api.lib" "%OUTPUT_DIR%\echodice_api.lib" || echo Warning: failed to copy echodice_api.lib
copy /Y "%ROOT_DIR%\lib\cpp\vcpkg\installed\x64-windows\bin\*.dll" "%OUTPUT_DIR%\" || echo Warning: failed to copy runtime DLLs

rem ─────────────────────────────────────────────
rem [5/6] Compilation Rust
echo.
echo [Build] [5/6] Building Rust (verbose)...
cd /d "%ROOT_DIR%\core\rust"
set "CARGO_TARGET_DIR=%BUILD_RUST_DIR%"
set "RUSTFLAGS=-L native=%OUTPUT_DIR% -l dylib=echodice_api"
echo   RUSTFLAGS=%RUSTFLAGS%

cargo build --release --verbose
if errorlevel 1 (
    echo.
    echo *** ERROR: Rust build failed ***
    exit /b 1
)

rem ─────────────────────────────────────────────
rem [6/6] Copie des exécutables Rust
echo.
echo [Build] [6/6] Copying executable & rlib to output
copy /Y "%BUILD_RUST_DIR%\release\echodice.exe"     "%OUTPUT_DIR%\" || echo Warning: failed to copy echodice.exe
copy /Y "%BUILD_RUST_DIR%\release\libechodice.rlib" "%OUTPUT_DIR%\" || echo Warning: failed to copy libechodice.rlib"

exit /b 0
