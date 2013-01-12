@echo off
REM Bitness of MapGuide we're testing with: 32|64
SET PLATFORM=32
REM Auto-start mgserver.exe, set to 0 if testing against a service-installed mgserver.exe: 1|0
SET START_MGSERVER=1
REM Time to wait for mgserver.exe to startup in seconds
SET MGSERVER_WAIT=15
REM Test only Web Extensions API, set to 0 to also test against the mapagent (must be already setup on a web server): 1|0
SET API_ONLY=1