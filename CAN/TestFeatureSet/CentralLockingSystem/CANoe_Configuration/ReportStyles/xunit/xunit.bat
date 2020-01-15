@echo off

:: =================================================================
:: xUnit conversion script
:: =================================================================

:: stylesheet processor should be located either under current 
:: directory or under a location specified in the PATH system vaiable

:: setting stylesheet processor
set proc=sabcmd.exe

:: check stylesheet processor path
call :check_exec %proc% & if errorlevel 1 goto :check_usage
call :check_path %proc% & if errorlevel 1 goto :check_usage
goto :eof

:: check input parameters
:check_usage
if "%~1"=="" (
  echo.
  echo Invalid number of parameters.
  echo.
  echo Usage:
  echo   %0 ^<input-file^> [^<ouput-file^>]
  echo Where:
  echo   ^<input-file^>         Location of CANoe report.
  echo   ^<output-file^>        Optional. Target location of xUnit report. 
  echo                          If not specified, the location of the xUnit report 
  echo                          will be: ^<input-file^>_xunit.xml
  goto :eof
)

:: check input file path
if not exist %1 (
  echo Input file not found: %1%
  goto :eof
)

:: check stylesheet path
set xslt=%~dp0testunit_report\xunit.xslt
call :check_xslt "%xslt%" & if errorlevel 1 goto :eof

:: set output file
if "%~2"=="" (
  call :set_out "%~dpn1_xunit.xml"
) else (
  call :set_out "%~2"
)
:: start the transformation
call :exec_proc "%xslt%" "%~1" %out% "%temp%\sabmcd.log"
goto :eof


:check_exec
  if exist "%~1" (
    exit /b 1
  ) else (
    echo Stylesheet processor %proc% not found on current dirrectory.
    exit /b 0
  )

:check_path
  set procpath=%~$PATH:1
  if "%procpath%"=="" (  
    echo Stylesheet processor %proc% not found under PATH system variable.
    exit /b 0
  ) else (
    exit /b 1
  )

:check_xslt
  if not exist %1 (
    echo Stylesheet not found: %1
    exit /b 1
  ) else (
    exit /b 0
  )
  
:: %1=ouput file
:set_out
  set out="%~1"
  goto :eof
  
::%1=xslt file, %2=input file, %3=output file, %4=log file
:exec_proc
  if exist %4 del %4
  %proc% "file://%~1" "file://%~2" "file://%~3" -L="%4"
  if ERRORLEVEL 1 (
    echo.
    echo An error occured during transformation. See logfile: %4
    echo.
  )
  ::more %log%
  goto :eof