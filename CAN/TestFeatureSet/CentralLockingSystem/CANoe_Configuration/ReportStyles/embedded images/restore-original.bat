@echo off
if exist "%~2\%~3_original.xml" move /Y "%~2\%~3_original.xml" "%~2\%~3.xml"
