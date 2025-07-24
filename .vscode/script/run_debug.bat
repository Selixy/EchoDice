@echo off
cls

REM Compilation
cd /d "%~dp0"
cd build
call build_Win64_Debug.bat
if errorlevel 1 exit /b 1
cls

REM Nettoyage
cd /d "%~dp0"
cd build
call clean.bat
if errorlevel 1 exit /b 1


REM Lancement du binaire
cd /d "%~dp0"
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    echo Architecture : Windows x64
    cd ..\..\build\EchoDice\Win64_Debug
) else if "%PROCESSOR_ARCHITECTURE%"=="ARM64" (
    echo Architecture : Windows ARM64
)
echo.
echo Lancement de EchoDice...
echodice.exe
