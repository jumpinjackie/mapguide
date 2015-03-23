@echo off
for /f "tokens=1*" %%i in ('svn status ^| find "?"') do del /q "%%j"
for /f "tokens=1*" %%i in ('svn status ^| find "?"') do rd /s /q "%%j"