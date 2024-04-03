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

:uninstall_wireshark
	set /p choice="Do you want to uninstall wireshark? (Y/n): "
	if /i "%choice%"=="y" (
		echo Uninstalling Wireshark...
		call C:\Program Files\Wireshark\uninstall-wireshark.exe
	) else if /i "%choice%"=="n" (
		echo leaving Wireshark installed.
	) else (
		echo Invalid choice. Please enter 'y' or 'n'.
		pause
		goto :uninstall_wireshark
	)

echo.
:: remove sslkeylog variable
echo Removing SSLKEYLOGFILE variable...
call remove_sslkeycapture.bat

echo.
::remove historylogger service
echo Removing historylogger service...

echo Uninstall completed!
echo.
pause
exit /b

:elevate
:: Self-elevate the script
powershell -Command "Start-Process cmd -ArgumentList '/c %~f0' -Verb runAs" >nul 2>&1
exit /b
