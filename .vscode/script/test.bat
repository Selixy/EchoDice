@echo off
cls


REM Compilation
cd /d "%~dp0"
cd build
call build_Win64_Debug.bat
if errorlevel 1 exit /b 1

REM Test
cd /d "%~dp0"
cd ../../core/rust
cargo test
rmdir /s /q target
if errorlevel 1 (
    echo Les tests ont échoué.
) else (
    echo Tous les tests sont passés avec succès.
)


REM Nettoyage
cd /d "%~dp0"
cd build
call clean.bat
if errorlevel 1 exit /b 1