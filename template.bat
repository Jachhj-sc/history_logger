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

:auto_install?
	set /p choice="Do you want to install automatically? (Y/n): "
	if /i "%choice%"=="y" (
		echo Automatic installation started.
	) else if /i "%choice%"=="n" (
		echo Manual installation started.
	) else (
		echo Invalid choice. Please enter 'y' or 'n'.
		pause
		goto :auto_install?
	)
	
pause

exit /b

:elevate
:: Self-elevate the script
powershell -Command "Start-Process cmd -ArgumentList '/c %~f0' -Verb runAs" >nul 2>&1
exit /b