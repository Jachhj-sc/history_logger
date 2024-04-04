@echo off
cd /d "%~dp0"
setlocal enabledelayedexpansion

:: Define the output file path
set "outputFile=adapter_list.txt"

:: Clear the output file at the start of the script
::if exist "%outputFile%" del "%outputFile%"
echo. > %outputFile%

:: Fetch network adapter configurations with IP enabled
for /f "tokens=*" %%a in ('wmic nicconfig where "IPEnabled=true" get Description^, IPAddress /format:list') do (
    set "line=%%a"
    set "line=!line:~0,-1!"

    :: Check for Description line
    if "!line:Description=!" neq "!line!" (
        set "description=!line:Description=!"
    )

    :: Check for IP Address line and process it
    if "!line:IPAddress=!" neq "!line!" (
        set "ipAddresses=!line:IPAddress={!"
        set "ipAddresses=!ipAddresses:}=!"
        set "ipAddresses=!ipAddresses:,"=" !"

        for %%i in (!ipAddresses!) do (
            set "ipAddress=%%~i"
            :: Skip IPv6 addresses to get the IPv4 address
            if "!ipAddress:~0,1!" neq "f" (
                :: Ping test using the IPv4 address
                ping -n 1 -S !ipAddress! google.com >nul
                if !errorlevel! == 0 (
                    echo Description=!description!, IPAddress=!ipAddress! >> !outputFile!
                    echo Found working adapter: !description! with IP !ipAddress!
                )
            )
        )
    )
)

echo Adapter identifiers and IP addresses have been written to "%outputFile%".

endlocal
