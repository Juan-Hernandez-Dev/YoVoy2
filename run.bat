@echo off
echo Configuring and building project...
cmake -B build -S . -G "MinGW Makefiles"
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b %errorlevel%
)

cmake --build build
if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b %errorlevel%
)

echo.
echo Running application...
echo ========================
for /r build %%i in (*.exe) do (
    "%%i"
    goto :done
)

:done
pause
