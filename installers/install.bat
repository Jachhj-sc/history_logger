@echo off

:: go to the script's directory
cd /d "%~dp0"

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
echo Starting installer...

:: var expansion delayed
setlocal enabledelayedexpansion

:: variables 
set "sslkeylog_path=%HOMEDRIVE%\tmp"
set "sslkeylog_file=sslkeys.log"

set "sslkeylog_path_file=sslkeylog_path.txt"

set "wireshark_path=%HOMEDRIVE%\Program Files\Wireshark"

set "historylog_path=%HOMEDRIVE%\tmp\history"
set "historylog_file=history.log"
set "auto_install=false"

set "h_logger_install_bin_path=%HOMEDRIVE%\Program Files\h_logger\bin"

set "bins_path=..\bins"
set "logger_service_executable=h_logger.exe"
set "logger_service_watchdog=h_logger_watchdog.exe"

::set "logger_service_watchdog_watchdog=h_logger_watchdog_watchdog.exe"

:auto_install?
	set choice=
	set /p choice="Do you want to install automatically? (Y/n): "
	if /i "!choice!"=="y" (
		echo Automatic installation started.
		set "auto_install=true"
	) else if /i "!choice!"=="n" (
		echo Manual installation started.
		set "auto_install=false"
	) else (
		echo Invalid choice. Please enter 'y' or 'n'.
		pause
		goto :auto_install?
	)

echo.

:: check if wireshark is installed
IF EXIST "%HOMEDRIVE%\Program Files\Wireshark\tshark.exe" (
	echo Wireshark 'tshark' is already installed.
	goto :install_sslkeylog_files
) ELSE (
	call :no_wireshark_info
	goto :install_wireshark
)

:no_wireshark_info
	echo Wireshark is not installed.
	echo Please install Wireshark. '.\installers\Wireshark-win64-3.4.6.exe'
	echo Make sure that Tshark is selected, in components to install
	echo And that the destination folder is 'C:\Program Files\Wireshark'.
	echo And when npcap is not installed yet, also select install npcap.
	echo.
	goto :eof

:install_wireshark
	if "%auto_install%"=="true" (
		goto :wireshark_auto_install
	) else (
		set /p choice="Do you want to install Wireshark now? (y/n): "

		if /i "!choice!"=="y" (
			goto :wireshark_auto_install
		) else if /i "!choice!"=="n" (
			echo Installation cancelled.
			echo.
::exit
			goto :end
		) else (
			echo Invalid choice. Please enter 'y' or 'n'.
			goto :install_wireshark
		)
	)	

:wireshark_auto_install
	echo.
	echo Installation of wireshark started.
	call .\installers\Wireshark-4.2.4-x64.exe
	echo.

:: check if succesfull
IF EXIST "%HOMEDRIVE%\Program Files\Wireshark\tshark.exe" (
	echo Wireshark 'tshark' is succesfully installed.
	goto :install_sslkeylog_files
) ELSE (
	echo Wireshark 'tshark' failed to be installed.
	call :no_wireshark_info
	goto :end
)

:install_sslkeylog_files
	if "%auto_install%"=="true" (
		goto :sslkeylog_auto_install
	) else (

		set /p choice="Do you want to install sslkeylog files? (y/n): "

		if /i "!choice!"=="y" (
			goto :sslkeylog_auto_install
		) else if /i "!choice!"=="n" (
			echo Installation of sslkeylog files cancelled.
			echo.
			goto :install_historylog_service
		) else (
			echo Invalid choice. Please enter 'y' or 'n'.
			goto :install_sslkeylog_files
		)
	)

:sslkeylog_auto_install
	echo.
	echo Installation of sslkeylog files started.

	call .\install_ssl_keylog_files.bat
	echo Done

:install_historylog_service
	if "%auto_install%"=="true" (
		goto :historylog_auto_install
	) else (
		set /p choice="Do you want to install the historylogger service? (y/n): "

		if /i "!choice!"=="y" (
			goto :historylog_auto_install
		) else if /i "!choice!"=="n" (
			echo Installation of historylog service cancelled.
			echo.
			goto :end
		) else (
			echo Invalid choice. Please enter 'y' or 'n'.
			goto :install_historylog_service
		)
	)

:historylog_auto_install
	echo Installation of the history log service started.

	IF EXIST "%h_logger_install_bin_path%" (
		echo Directory "%h_logger_install_bin_path%" exists.
	) ELSE (
		echo Directory "%h_logger_install_bin_path%" does not exist.
		mkdir "%h_logger_install_bin_path%"
		echo Directory "%h_logger_install_bin_path%" created.
	)

::copy binaries to bin path
	IF EXIST "%bins_path%\%logger_service_executable%" (
		echo service in "%bins_path%" exists.
		copy "%bins_path%\%logger_service_executable%" "%h_logger_install_bin_path%"
	) ELSE (
		echo service in "%bins_path%" does not exist. FATAL ERROR! contact developer.
		goto :end
	)

	IF EXIST "%bins_path%\%logger_service_watchdog%" (
		echo service_watchdog in "%bins_path%" exists.
		copy "%bins_path%\%logger_service_watchdog%" "%h_logger_install_bin_path%"
	) ELSE (
		echo service_watchdog in "%bins_path%" does not exist. FATAL ERROR! contact developer.
		goto :end
	)

:: add bin path to the system PATH variable
	call "%bins_path%\append_to_path.exe" "%h_logger_install_bin_path%"
	call "%bins_path%\append_to_path.exe" "%wireshark_path%"

	echo.
	echo writing sslkeylog path to files...
	echo %SSLKEYLOGFILE% > %sslkeylog_path_file%
	copy "%sslkeylog_path_file%" "%h_logger_install_bin_path%"

:: create service_watchdog task
	::schtasks /create /tn "h_logger Watchdog" /tr "wscript.exe //b \"%h_logger_install_bin_path%\%noshell_helper%\" \"%h_logger_install_bin_path%\%logger_service_watchdog%\" \"%logger_service_executable%\" \"60000\" \"%h_logger_install_bin_path%\%noshell_helper%\"" /sc ONLOGON /rl HIGHEST /ru "%USERNAME%"
	schtasks /create /tn "h_logger Watchdog" /tr "\"%h_logger_install_bin_path%\%logger_service_watchdog%\" \"%logger_service_executable%\" \"30000\"" /sc ONLOGON /rl HIGHEST /ru "%USERNAME%"
	schtasks /run /tn "h_logger Watchdog"

	echo.
	echo Installation of the history log service completed

echo Done
goto :end

:elevate
:: Self-elevate the script
powershell -Command "Start-Process cmd -ArgumentList '/c %~f0' -Verb runAs" >nul 2>&1
exit /b

:end
echo Press a key to exit...
pause > nul
exit /b