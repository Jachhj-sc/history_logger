@echo off
:: go to the script's directory
cd /d "%~dp0"

:: Check for administrative privileges
net session >nul 2>&1
if %errorlevel% == 0 (
    ::echo Running with administrative privileges
    goto :admin
) else (
    ::echo Requesting administrative privileges...
    goto :elevate
)

:admin
:: Your script's admin commands go here

::delete logger service

echo.
echo Done
pause
exit /b

:elevate
:: Self-elevate the script
powershell -Command "Start-Process cmd -ArgumentList '/c %~f0' -Verb runAs" >nul 2>&1
exit /b