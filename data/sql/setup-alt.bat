@echo off

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: build_server.bat
::
:: Build's the project and produces a directory of all necesary executables and
:: dll's needed to run an instance of the server.
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: initialize build variables
set "PROJECT_BASE=%~dp0"
set MYSQL=mysql
set DBHOST=localhost
set DBUSER=
set DBPASS=

goto :PROCESS_ARGUMENTS
:CONTINUE_FROM_PROCESS_ARGUMENTS

call :CREATE_DATABASE galaxy_manager
call :CREATE_DATABASE galaxy

goto :EOF

rem ----------------------------------------------------------------------------
rem --- Start of PROCESS_ARGUMENTS ---------------------------------------------
:PROCESS_ARGUMENTS
    if "%~0" == "" goto :CONTINUE_FROM_PROCESS_ARGUMENTS


    if "%~0" == "-h" (
    	echo setup-db.bat Help
    	echo.
    	echo "    /mysql [path]         Specifies a custom path to the mysql client"
    	echo "    /host  [hostname]     Mysql hostname"
    	echo "    /user  [username]     Mysql username"
    	echo "    /pass  [password]     Mysql password"
    )

    if "%~0" == "/mysql" (
        set MYSQL=%~1
        shift
    )

    if "%~0" == "/host" (
        set DBHOST=%~1
        shift
    )

    if "%~0" == "/user" (
        set DBUSER=%~1
        shift
    )
    
    if "%~0" == "/pass" (
        set DBPASS=%~1
        shift
    )
    
    shift
goto :PROCESS_ARGUMENTS

rem ----------------------------------------------------------------------------
rem --- Start of CREATE_DATABASE -------------------------------------------------
:CREATE_DATABASE

set DBNAME=%~1

    :: Create the schema

    cd "%PROJECT_BASE%%DBNAME%"
	%MYSQL% --password=%DBPASS% --host=%DBHOST% --user=%DBUSER% --default-character-set=utf8 < "create.sql"

    cd "%PROJECT_BASE%"
    cd "%PROJECT_BASE%%DBNAME%\scripts"
	for /F %%A IN ('dir /b "*.sql" ^| sort') do (
		%MYSQL% --password=%DBPASS% --host=%DBHOST% --user=%DBUSER% --database=%DBNAME% --default-character-set=utf8 < "%%A"
	    ECHO. Installing %%A [Done]
	)
	
    cd "%PROJECT_BASE%%DBNAME%\functions"
	for /F %%A IN ('dir /b "*.sql" ^| sort') do (
		%MYSQL% --password=%DBPASS% --host=%DBHOST% --user=%DBUSER% --database=%DBNAME% --default-character-set=utf8 < "%%A"
	    ECHO. Installing %%A [Done]
	)
	
    cd "%PROJECT_BASE%%DBNAME%\procedures"
	for /F %%A IN ('dir /b "*.sql" ^| sort') do (
		%MYSQL% --password=%DBPASS% --host=%DBHOST% --user=%DBUSER% --database=%DBNAME% --default-character-set=utf8 --comments < "%%A"
	    ECHO. Installing %%A [Done]
	)	
goto :EOF