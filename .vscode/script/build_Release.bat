@echo off
cls
cd /d "%~dp0"

rem ─────────────────────────────────────────────
rem Écran vide temporaire (évite affichage résiduel)
for /L %%i in (1,1,100) do echo.
cls

rem ─────────────────────────────────────────────
echo [1/4] Nettoyer les compilations...

REM Calcul du chemin absolu vers ..\..\build
set "REL_PATH=%CD%\..\..\build"
for %%I in ("%REL_PATH%") do set "BUILD_DIR=%%~fI"

echo [DEBUG] BUILD_DIR = %BUILD_DIR%

REM Supprimer le dossier build s'il existe
if exist "%BUILD_DIR%" (
    rd /s /q "%BUILD_DIR%" || (
        echo [ERROR] Impossible de supprimer %BUILD_DIR%
        exit /b 1
    )
)

REM Recréer le dossier build
mkdir "%BUILD_DIR%" || (
    echo [ERROR] Impossible de créer %BUILD_DIR%
    exit /b 1
)

echo [OK] Dossier %BUILD_DIR% recréé avec succès.

rem ─────────────────────────────────────────────
echo [2/4] Compilation pour Windows x64/x86
cd /d "%~dp0"
cd build
call build_Win64.bat

rem ─────────────────────────────────────────────
echo [3/4] Compilation pour ...

rem ─────────────────────────────────────────────
echo [4/4] Clean
cd /d "%~dp0"
call clean.bat