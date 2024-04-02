@echo off
:: Check for administrative privileges
net session >nul 2>&1
if %errorlevel% == 0 (
    echo Running with administrative privileges
    goto :admin
) else (
    echo Requesting administrative privileges...
    goto :elevate
)

:admin
:: Your script's admin commands go here
echo Performing admin tasks...

pause
exit

:elevate
:: Self-elevate the script
powershell -Command "Start-Process cmd -ArgumentList '/c %~f0' -Verb runAs" >nul 2>&1
exit /b
