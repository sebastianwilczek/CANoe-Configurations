@echo off

move /Y "%~2\%~3.xml" "%~2\%~3_original.xml"
InlineReportImages.exe "%~2\%~3_original.xml" "%~2\%~3.xml" "%~2"
if %ERRORLEVEL% EQ 0 goto exit

:error
pause "Embedding images in report failed. Keeping original report."
move /Y "%~2\%~3_original.xml" "%~2\%~3.xml"

:exit
