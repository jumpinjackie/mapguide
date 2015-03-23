@echo off
SET API_KEY=%1
NuGet.exe SetApiKey %API_KEY%
for /f %%i in ('dir /b *.nupkg') do (NuGet.exe push %%i)