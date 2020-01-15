@echo off
cscript.exe "%~dp0AutomaticTest.vbs" "..\..\CentralLockingSystem.cfg" "AutomaticTests.log"

rem Output number of errors, and signal success/failure to Jenkins
if errorlevel 1 (
	echo %errorlevel% failed test configurations
	exit /b %errorlevel%
) else (
	echo all test units successfully executed
)
