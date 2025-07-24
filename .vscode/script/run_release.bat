@echo off
cls
for /L %%i in (1,1,100) do echo.
cls


REM Compilation
cd /d "%~dp0"
cd build
call build_Win64.bat
if errorlevel 1 exit /b 1

REM Nettoyage
cd /d "%~dp0"
cd build
call clean.bat
if errorlevel 1 exit /b 1


REM Lancement du binaire
cd /d "%~dp0"
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    echo Architecture : Windows x64
    cd ..\..\build\EchoDice\Win64
) else if "%PROCESSOR_ARCHITECTURE%"=="ARM64" (
    echo Architecture : Windows ARM64
)
echo.
echo Lancement de EchoDice...
echodice.exe
