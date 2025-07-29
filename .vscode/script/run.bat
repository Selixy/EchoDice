@echo off
cls

rem ─────────────────────────────────────────────
rem Écran vide temporaire (évite affichage résiduel)
for /L %%i in (1,1,100) do echo.
cls

rem ─────────────────────────────────────────────
echo [1/4] Compilation
cd /d "%~dp0"
cd build
call build_Win64.bat
if errorlevel 1 exit /b 1


rem ─────────────────────────────────────────────
echo [2/4] étection de l architecture
cd /d "%~dp0"
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    echo Architecture : Windows x64
    set "OUTPUT_DIR=..\..\build\EchoDice"
) else if "%PROCESSOR_ARCHITECTURE%"=="ARM64" (
    echo Architecture : Windows ARM64
    set "OUTPUT_DIR=..\..\build\EchoDice_ARM64"
) else (
    echo [ERROR] Architecture non reconnue : %PROCESSOR_ARCHITECTURE%
    set "OUTPUT_DIR=..\..\build\EchoDice"
)
echo.


rem ─────────────────────────────────────────────
echo [3/4] Verification des dépendances DLL

for /f "usebackq tokens=1,* delims==" %%I in ("..\..\.env\path") do set "%%I=%%~J"
if exist "%OUTPUT_DIR%\echodice.exe" (
    "%DEPENDENCIES%" -imports "%OUTPUT_DIR%\echodice.exe" > deps_check.txt 2>&1

    findstr /C:"Error opening file" deps_check.txt > nul
    if !errorlevel! equ 0 (
        echo [ERROR] Dependances manquantes détectées :
        findstr /C:"Error opening file" deps_check.txt
        echo.
    ) else (
        echo [4/4] Lancement de EchoDice
        cd /d "%OUTPUT_DIR%"
        echodice.exe
    )

    del deps_check.txt > nul 2>&1
) else (
    echo [ERROR] echodice.exe introuvable dans %OUTPUT_DIR%
)
