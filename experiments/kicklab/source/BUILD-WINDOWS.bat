@echo off
setlocal
cd /d "%~dp0"
where cmake >nul 2>nul
if errorlevel 1 (
  echo Installer CMake et Visual Studio 2022 Build Tools, composant Developpement Desktop C++.
  pause
  exit /b 1
)
cmake -S . -B build -A x64
if errorlevel 1 goto failure
cmake --build build --config Release
if errorlevel 1 goto failure
ctest --test-dir build -C Release --output-on-failure
if errorlevel 1 goto failure
echo Executable : build\Release\KickLab.exe
pause
exit /b 0
:failure
echo La compilation ou les tests ont echoue. Voir le message ci-dessus.
pause
exit /b 1
