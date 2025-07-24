@echo off
REM Se place dans le dossier racine du projet
cd /d "%~dp0\..\..\.."

REM Va dans le dossier build
cd build


REM Supprime tous les fichiers à la racine de /build
for %%f in (*) do (
    del /f /q "%%f" 2>nul
)

REM Supprime tous les dossiers sauf ceux commençant par EchoDice
for /d %%d in (*) do (
    echo %%d | findstr /B /C:"EchoDice" >nul
    if errorlevel 1 (
        rmdir /s /q "%%d"
    )
)




echo Clean termine avec succes.
exit /b 0

