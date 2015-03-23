@echo off
REM Bitness of MapGuide we're testing with: 32|64
SET PLATFORM=64
REM Auto-start mgserver.exe, set to 0 if testing against a service-installed or already running mgserver.exe: 1|0
SET START_MGSERVER=0
REM Time to wait for mgserver.exe to startup in seconds
SET MGSERVER_WAIT=15
REM Test only Web Extensions API, set to 0 to also test against the mapagent (must be already setup on a web server): 1|0
SET API_ONLY=0
REM Vars to inject into $_SERVER in PHP CLI
SET SERVER_ADDR=localhost
REM SET SERVER_PORT=8008
REM SET WEBCONFIGINI=%CD%\..\Web\src\webconfig.ini
SET SERVER_PORT=8018
SET WEBCONFIGINI=D:\mg-trunk-vc10\MgDev\Release64\Web\www\webconfig.ini
