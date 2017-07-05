@echo off
if "%ORG_PATH%" == "" set ORG_PATH=%PATH%

PATH=c:\Program Files\Git\bin;c:\Linaro-gcc\bin;c:\cygwin\bin;%ORG_PATH%
echo Set the PATH for building.

echo Use "set TARGET_IP=192.168.10.19" to copy files to target by make copy.
echo Or, Use "make copy TARGET_IP=192.168.10.19" to copy files to target."

