@echo off
setlocal enableextensions enabledelayedexpansion
echo.

REM ─────────────────────────────────────────────
REM 1) Charger les variables d’environnement
cd /d "%~dp0\..\.."
for /f "usebackq tokens=1,* delims==" %%I in (".env\path") do set "%%I=%%~J"

set "ROOT_DIR=%CD%"
set "BUILD_DIR=%ROOT_DIR%\build"
set "OUTPUT_DIR=%BUILD_DIR%\EchoDice"

REM ─────────────────────────────────────────────
REM 2) Analyse des DLL requises avec Dependencies
echo [Clean] [1/4] Analyse des DLL requises...
if not exist "%DEPENDENCIES%" (
    echo   [WARN] Dependencies.exe introuvable: %DEPENDENCIES%
    goto :end
)

if not exist "%OUTPUT_DIR%\echodice.exe" (
    echo   [WARN] echodice.exe introuvable dans %OUTPUT_DIR%
    goto :end
)

"%DEPENDENCIES%" -modules "%OUTPUT_DIR%\echodice.exe" > "%OUTPUT_DIR%\deps_used.txt"

REM ─────────────────────────────────────────────
REM 3) Suppression des DLL superflues...
echo [Clean] [2/4] Suppression des DLL superflues...

type "%OUTPUT_DIR%\deps_used.txt" | findstr /I ".dll" > "%OUTPUT_DIR%\deps_filtered.txt"

for %%f in ("%OUTPUT_DIR%\*.dll") do (
    findstr /I /C:"%%~nxf" "%OUTPUT_DIR%\deps_filtered.txt" > nul
    if errorlevel 1 (
        echo    Supprime: %%~nxf
        del /f /q "%%~f"
    )
)

REM ─────────────────────────────────────────────
REM 4) Suppression des éléments de compilation
echo [Clean] [3/4] Suppression des elements de compilation...
del /f /q "%OUTPUT_DIR%\*.lib" > nul 2>&1

REM Supprimer tous les dossiers/fichiers de build\ sauf EchoDice\
for /d %%D in ("%BUILD_DIR%\*") do (
    if /I not "%%~nxD"=="EchoDice" (
        echo    Supprime dossier : %%~nxD
        rd /s /q "%%~fD"
    )
)

for %%F in ("%BUILD_DIR%\*") do (
    if not exist "%%~fF\" (
        echo    Supprime fichier : %%~nxF
        del /f /q "%%~fF"
    )
)

REM ─────────────────────────────────────────────
REM 5) Nettoyage des fichiers temporaires
:end
del "%OUTPUT_DIR%\deps_used.txt" > nul 2>&1
del "%OUTPUT_DIR%\deps_filtered.txt" > nul 2>&1

echo.
echo Clean termine avec succès.
exit /b 0
