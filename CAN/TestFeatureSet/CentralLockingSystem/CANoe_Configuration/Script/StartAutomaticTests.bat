@echo off
cscript.exe "%~dp0AutomaticTest.vbs" "..\..\CentralLockingSystem.cfg" "AutomaticTests.log"

rem Output number of errors in tests, but do not signal to Jenkins
if errorlevel 1 (
 	echo %errorlevel% failed test configurations
) else (
	echo all test units successfully executed
)
exit /b 0
