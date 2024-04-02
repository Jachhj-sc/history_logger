@echo off
reg delete "HKCU\Environment" /v SSLKEYLOGFILE /f
echo SSLKEYLOGFILE cleared
pause