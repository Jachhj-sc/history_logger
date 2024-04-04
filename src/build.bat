@echo off
cd /d "%~dp0"

clang -o ..\bins\h_logger.exe h_logger.c -Wl,/subsystem:windows -lshell32
clang -o ..\bins\h_logger_watchdog.exe h_logger_watchdog.c -Wl,/subsystem:windows -lshell32
clang append_to_path.c -o ..\bins\append_to_path.exe -ladvapi32 -luser32
clang stealth_get_active_network_adapter.c -o ..\bins\stealth_get_active_network_adapter.exe -Wl,/subsystem:windows -liphlpapi -lws2_32