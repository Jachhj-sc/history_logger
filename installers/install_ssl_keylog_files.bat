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

IF EXIST "%sslkeylog_path%" (
	echo Directory %sslkeylog_path% exists.
) ELSE (
	echo Directory %sslkeylog_path% does not exist.
	mkdir %sslkeylog_path%
	echo Directory %sslkeylog_path% created.
)

IF EXIST "%historylog_path%" (
	echo Directory %historylog_path% exists.
) ELSE (
	echo Directory %historylog_path% does not exist.
	mkdir %historylog_path%
	echo Directory %historylog_path% created.
)

:: touch files
echo.>>%sslkeylog_path%\%sslkeylog_file%
echo.>>%historylog_path%\%historylog_file%

echo.
echo installing sslkeylog variable...
setx SSLKEYLOGFILE "%sslkeylog_path%\%sslkeylog_file%"

echo wrote SSLKEYLOGFILE: %SSLKEYLOGFILE%
echo %SSLKEYLOGFILE% > ..\sslkeylog_path.txt
echo Done

exit /b

:elevate
:: Self-elevate the script
powershell -Command "Start-Process cmd -ArgumentList '/c %~f0' -Verb runAs" >nul 2>&1
exit /b