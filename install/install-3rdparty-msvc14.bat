set "ROOT_DIR=%CD%"

set "BUILD_LOG=%ROOT_DIR%\..\build.log"
set "CONFIG_LOG=%ROOT_DIR%\..\config.log"
set "FAILURES_LOG="%ROOT_DIR%\..\failures.log"

::  Setting visual studio environment
::  =================================

IF DEFINED TERRAMA_X64 ( 
set "_config=x64"  
set "_X86=_x64"
set "PATH=%CMAKE_FILEPATH%;%QMAKE_FILEPATH%;%ROOT_DIR%\icu-58.2-vs2015\bin64;%ROOT_DIR%\curl-7.42.1\binaries%_X86%\lib;%PATH%"
set _CMAKE_GENERATOR="Visual Studio 14 2015 Win64" 
) ELSE ( 
set "_config=x86"
set "_X86=_x86"
set "PATH=%CMAKE_FILEPATH%;%QMAKE_FILEPATH%;%ROOT_DIR%\icu-58.2-vs2015\bin;%ROOT_DIR%\curl-7.42.1\binaries%_X86%\lib;%PATH%"
set _CMAKE_GENERATOR="Visual Studio 14 2015" 
)

set MSVC_FOLDER=msvc14
set MSVC_VERSION=1900
set MSVC_TOOLSET=v140

echo | set /p="Configuring visual studio... "<nul

call "%VCVARS_FILEPATH%"\vcvarsall.bat %_config%

echo done.
echo.

::  ================================

goto begin_libs

:: Building libraries
::  ==================

:append_log_begin
SETLOCAL
echo ********************** >> %CONFIG_LOG% 2>nul
echo %1 >> %CONFIG_LOG% 2>nul
echo ********************** >> %CONFIG_LOG% 2>nul

echo ********************** >> %BUILD_LOG% 2>nul
echo %1 >> %BUILD_LOG% 2>nul
echo ********************** >> %BUILD_LOG% 2>nul
goto :EOF
ENDLOCAL


:append_log_end
SETLOCAL
echo. >> %CONFIG_LOG% 2>nul
echo ********************** >> %CONFIG_LOG% 2>nul
echo. >> %CONFIG_LOG% 2>nul
echo. >> %CONFIG_LOG% 2>nul

echo. >> %BUILD_LOG% 2>nul
echo ********************** >> %BUILD_LOG% 2>nul
echo. >> %BUILD_LOG% 2>nul
echo. >> %BUILD_LOG% 2>nul

goto :EOF
ENDLOCAL

:skip_build
SETLOCAL
echo skip.

goto :EOF
ENDLOCAL

:remove_lib
SETLOCAL
del %TERRAMA_DEPENDENCIES_DIR%\lib\*%1* /S /Q >nul 2>nul

goto :EOF
ENDLOCAL

:: 1-Nome da biblioteca
:: 2-Mensagem de erro simples.
:: 3-Desvio, nome do atalho para a próxima biblioteca

:buildFailLog
SETLOCAL
set FAIL=1
echo fail on %2.
echo %1: fail on %2. >>%FAILURES_LOG%  

goto :EOF
ENDLOCAL


:begin_libs

set LIBS_DIR=%TERRAMA_DEPENDENCIES_DIR%\lib
set INCLUDES_DIR=%TERRAMA_DEPENDENCIES_DIR%\include

:: libraries not linked against TerraLib 5 (NOT fully installed)
:: -------------------------------------------------------------
set EXPAT=%LIBS_DIR%\expat.dll
set EAY=%LIBS_DIR%\libeay32.dll
set EAYD=%LIBS_DIR%\libeay32d.dll
set SSL=%LIBS_DIR%\ssleay32.dll
set SSLD=%LIBS_DIR%\ssleay32d.dll
set ZLIB=%LIBS_DIR%\zlib.dll
set ZLIBD=%LIBS_DIR%\zlibd.dll
set READLINE=%LIBS_DIR%\readline-32.dll
set READLINED=%LIBS_DIR%\readline-32d.dll
set PCRE=%LIBS_DIR%\pcre.dll
set PCRED=%LIBS_DIR%\pcred.dll
set FREEXL=%LIBS_DIR%\freexl.dll
set FREEXLD=%LIBS_DIR%\freexld.dll
set PNG=%LIBS_DIR%\libpng15.dll
set HDF4=%LIBS_DIR%\hdfdll.dll
set TIFF=%LIBS_DIR%\libtiff.dll
set GEOTIFF=%LIBS_DIR%\libgeotiff.dll
set ICU=%LIBS_DIR%\icuuc58.dll
set XML2=%LIBS_DIR%\libxml2.dll
set NETCDF=%LIBS_DIR%\netcdf.dll

set LIBKML=%ROOT_DIR%\libkml-master\build%_X86%\Release\libkml.lib
set BZIP=%ROOT_DIR%\bzip2-1.0.6\lib%_X86%\libbz2.lib
set JPEG=%ROOT_DIR%\jpeg-9a\build%_X86%\libjpeg.lib
set MINIZIP=%ROOT_DIR%\unzip101e\build%_X86%\Release\minizip.lib

set "_aux=_x86"
IF DEFINED TERRALIB_X64 set "_aux=_x64"
set URIPARSER=%ROOT_DIR%\uriparser-0.8.4\win32\build%_aux%\uriparser.lib

:: libraries linked against TerraLib 5 (fully installed)
:: -------------------------------------------------------------
set ICONV=%LIBS_DIR%\iconv.lib
set PROJ=%LIBS_DIR%\proj_i.lib
set GEOS=%LIBS_DIR%\geos_i.lib
set XERCES=%LIBS_DIR%\xerces-c_3.lib
set BOOST=%LIBS_DIR%\boost_system-mt.lib
set PGIS=%LIBS_DIR%\libpqdll.lib
set SQLITE=%LIBS_DIR%\sqlite3.lib
set SPATIALITE=%LIBS_DIR%\spatialite_i.lib
set GDAL=%LIBS_DIR%\gdal_i.lib
set PROPERTYBROWSER=%LIBS_DIR%\qt_property_browser.lib
set QWT=%LIBS_DIR%\qwt.lib
set LUA=%LIBS_DIR%\lua.lib
set QTLUA=%LIBS_DIR%\qtlua.lib
set QTLUAE=%LIBS_DIR%\qtluae.lib
set TERRALIB4=%LIBS_DIR%\terralib.lib
set CURL=%LIBS_DIR%\libcurl.lib
set QSCINTILLA=%LIBS_DIR%\qscintilla2.lib
set GNUTLS=%LIBS_DIR%\libgnutls-26.lib
set GSASL=%LIBS_DIR%\libgsasl-7.lib

del %ROOT_DIR%\..\*.log /S /Q >nul 2>nul

IF NOT EXIST %LIBS_DIR% (
  mkdir %INCLUDES_DIR% >nul 2>nul 
  mkdir %LIBS_DIR% >nul 2>nul
  goto begin_build
)

goto openssl_deps

:begin_build

:openssl

:: ====
:: Openssl
set OPENSSL_DIR=%TERRALIB_DEPENDENCIES_DIR%\openssl-master
set OPENSSL_DEBUG_LIBRARIES=%OPENSSL_DIR%\out64dll.dbg
set OPENSSL_RELEASE_LIBRARIES=%OPENSSL_DIR%\out64dll
set OPENSSL_INCLUDE_DIR=%OPENSSL_DIR%\inc64

:: Check dependencies
goto end_openssl_deps
:openssl_deps
goto iconv_deps
:end_openssl_deps

echo | set /p="Installing openssl... "<nul

IF EXIST %SSL% call :skip_build && goto iconv 

call :append_log_begin openssl

:begin_openssl

xcopy %OPENSSL_DEBUG_LIBRARIES% %LIBS_DIR% /S /Y >nul 2>nul

xcopy %OPENSSL_RELEASE_LIBRARIES% %LIBS_DIR% /S /Y >nul 2>nul

xcopy %OPENSSL_INCLUDE_DIR% %INCLUDES_DIR% /S /Y >nul 2>nul

call :append_log_end openssl

:end_openssl

echo done.

cd %ROOT_DIR%
:: ====

:iconv

:: ====
:: Iconv
IF DEFINED TERRAMA_X64 ( 
  set "_libF=x64" 
) ELSE set "_libF=x86"

set ICONV_DIR=%TERRALIB_DEPENDENCIES_DIR%\iconv\%MSVC_FOLDER%
set ICONV_DEBUG_LIBRARIES=%ICONV_DIR%\%_libF%\Debug
set ICONV_RELEASE_LIBRARIES=%ICONV_DIR%\%_libF%\Release
set ICONV_INCLUDE_DIR=%ICONV_DIR%\myIconv\include

:: Check dependencies
goto end_iconv_deps
:iconv_deps
goto gnu_deps
:end_iconv_deps

echo | set /p="Installing iconv... "<nul

IF EXIST %ICONV% call :skip_build && goto gnu 

call :append_log_begin iconv

:begin_iconv

xcopy %ICONV_DEBUG_LIBRARIES% %LIBS_DIR% /S /Y >nul 2>nul

xcopy %ICONV_RELEASE_LIBRARIES% %LIBS_DIR% /S /Y >nul 2>nul

xcopy %ICONV_INCLUDE_DIR% %INCLUDES_DIR% /S /Y >nul 2>nul

call :append_log_end iconv

:end_iconv

echo done.

cd %ROOT_DIR%
:: ====

:gnu

:: ====
:: Gnu
set GNU_DIR=%ROOT_DIR%\gsasl-1.5.0

:: Check dependencies
goto end_gnu_deps
:gnu_deps
goto zlib_deps
:end_gnu_deps

echo | set /p="Installing gnu... "<nul

IF EXIST %GSASL% call :skip_build && goto zlib 

call :append_log_begin gnu

:begin_gnu

xcopy %GNU_DIR%\include %TERRAMA_DEPENDENCIES_DIR%\include /S /Y >nul 2>nul

xcopy %GNU_DIR%\lib\libgsasl* %TERRAMA_DEPENDENCIES_DIR%\lib /S /Y >nul 2>nul

xcopy %GNU_DIR%\lib\libgnutls* %TERRAMA_DEPENDENCIES_DIR%\lib /S /Y >nul 2>nul

call :append_log_end gnu

:end_gnu

echo done.

cd %ROOT_DIR%
:: ====

:zlib

:: ZLIB
::  ====
set ZL_DIR=%TERRAMA_DEPENDENCIES_DIR%\zlib-1.2.8
set ZL_INCLUDE_DIR=%ZL_DIR%;%ZL_DIR%\build%_X86%
set ZL_LIBRARY=%ZL_DIR%\build%_X86%\Release\zlib.lib
set ZLD_LIBRARY=%ZL_DIR%\build%_X86%\Debug\zlibd.lib
set ZL_LIBRARIES=debug;%ZLD_LIBRARY%;optimized;%ZL_LIBRARY%
set ZL_LIBRARIES_PATH=%ZL_LIBRARY%;%ZLD_LIBRARY%

:: Check dependencies
goto end_zlib_deps
:zlib_deps
goto quazip_deps
:end_zlib_deps

echo | set /p="Installing zlib... "<nul

IF EXIST %ZLIB% call :skip_build && goto quazip 

call :append_log_begin zlib

:begin_zlib

cd %ZL_DIR%

del build%_X86% /S /Q >nul 2>nul

mkdir build%_X86% >nul 2>nul

cd build%_X86% >nul 2>nul

( cmake -G %_CMAKE_GENERATOR% -DCMAKE_INSTALL_PREFIX=%TERRAMA_DEPENDENCIES_DIR% -DINSTALL_BIN_DIR=%TERRAMA_DEPENDENCIES_DIR%\lib %ZL_DIR% >>%CONFIG_LOG% 2>nul ) || call :buildFailLog  zlib "configuring" && goto quazip

( msbuild /m /p:Configuration=Release INSTALL.vcxproj >>%BUILD_LOG% 2>nul ) || call :buildFailLog zlib "build release" && goto quazip

( msbuild /m INSTALL.vcxproj >>%BUILD_LOG% 2>nul ) || call :buildFailLog zlib "build debug" && goto quazip

call :append_log_end zlib

:end_zlib  

echo done.

cd %ROOT_DIR%
::  ====

:quazip

::  Quazip
set QUAZIP_DIR=%ROOT_DIR%\quazip-0.7.3
set QUAZIP_INCLUDE_DIR=%TERRAMA_DEPENDENCIES_DIR%\include
set QUAZIP_LIBRARY=%TERRAMA_DEPENDENCIES_DIR%\lib\quazip5.lib

:: Check dependencies
goto end_quazip_deps
:quazip_deps
goto vmime_deps
:end_quazip_deps

echo | set /p="Installing quazip... "<nul

IF EXIST %QUAZIP_LIBRARY% call :skip_build && goto vmime 

call :append_log_begin quazip

:begin_quazip

cd %QUAZIP_DIR% >nul 2>nul

del build%_X86%_release /S /Q >nul 2>nul

mkdir build%_X86%_release >nul 2>nul

cd build%_X86%_release >nul 2>nul

( cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_INSTALL_PREFIX=%TERRAMA_DEPENDENCIES_DIR% ^
-DCMAKE_PREFIX_PATH="%QMAKE_FILEPATH%\..\lib\cmake" ^
-DQT_QTCORE_LIBRARY="%QMAKE_FILEPATH%\..\lib\Qt5Core.lib" ^
-DZLIB_INCLUDE_DIRS="%INCLUDES_DIR%" ^
-DZLIB_LIBRARIES="debug;%LIBS_DIR%\zlibd.lib;optimized;%LIBS_DIR%\zlib.lib" %QUAZIP_DIR% >>%CONFIG_LOG% 2>nul ) || call :buildFailLog quazip "configuring" && goto vmime

( nmake >>%BUILD_LOG% 2>nul ) || call :buildFailLog quazip "building" && goto vmime 

xcopy *.lib %LIBS_DIR% /Y >nul 2>nul

xcopy *.dll %LIBS_DIR% /Y >nul 2>nul

cd %QUAZIP_DIR% >nul 2>nul

del build%_X86%_debug /S /Q >nul 2>nul

mkdir build%_X86%_debug >nul 2>nul

cd build%_X86%_debug >nul 2>nul

( cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_INSTALL_PREFIX=%TERRAMA_DEPENDENCIES_DIR% ^
-DCMAKE_PREFIX_PATH="%QMAKE_FILEPATH%\..\lib\cmake" ^
-DQT_QTCORE_LIBRARY="%QMAKE_FILEPATH%\..\lib\Qt5Core.lib" ^
-DZLIB_INCLUDE_DIRS="%INCLUDES_DIR%" ^
-DZLIB_LIBRARIES="debug;%LIBS_DIR%\zlibd.lib;optimized;%LIBS_DIR%\zlib.lib" %QUAZIP_DIR% >>%CONFIG_LOG% 2>nul ) || call :buildFailLog quazip "configuring" && goto vmime

( nmake >>%BUILD_LOG% 2>nul ) || call :buildFailLog quazip "building" && goto vmime 

xcopy *.lib %LIBS_DIR% /Y >nul 2>nul

xcopy *.dll %LIBS_DIR% /Y >nul 2>nul

xcopy %QUAZIP_DIR%\quazip %INCLUDES_DIR%\quazip /Y /I >nul 2>nul

call :append_log_end quazip

:end_quazip

echo done.

cd %ROOT_DIR%
::  ====

:vmime

:: VMime
set VMIME_DIR=%ROOT_DIR%\vmime-master
set VMIME_INCLUDE_DIR=%TERRAMA_DEPENDENCIES_DIR%\include
set VMIME_LIBRARY=%LIBS_DIR%\vmime.lib
set VMIMED_LIBRARY=%LIBS_DIR%\vmimed.lib

:: Check dependencies
goto end_vmime_deps
:vmime_deps
goto begin_build
:end_vmime_deps

echo | set /p="Installing vmime... "<nul

IF EXIST %VMIMED_LIBRARY% call :skip_build && goto clean_third_directory

call :append_log_begin vmime

:begin_vmime

cd %VMIME_DIR% >nul 2>nul

mkdir %TERRAMA_DEPENDENCIES_DIR%\vmime >nul 2>nul
mkdir build%_X86%_debug >nul 2>nul
mkdir build%_X86%_release >nul 2>nul

cd build%_X86%_debug >nul 2>nul

( cmake -G %_CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_DEBUG_POSTFIX:STRING="d" ^
-DCMAKE_PREFIX_PATH="%TERRAMA_DEPENDENCIES_DIR%" ^
-DCMAKE_INSTALL_PREFIX="%TERRAMA_DEPENDENCIES_DIR%" ^
-DGNUTLS_INCLUDE_DIR="%INCLUDES_DIR%" -DGNUTLS_LIBRARY="%GNUTLS%" -DGSASL_LIBRARIES="%GSASL%" ^
-DVMIME_SENDMAIL_PATH="C:\VMime-SendMail" ^
 %VMIME_DIR% >>%CONFIG_LOG% 2>nul ) || call :buildFailLog vmime "configuring" && goto clean_third_directory

( msbuild /m /p:Configuration=Debug vmime.vcxproj >>%BUILD_LOG% 2>nul ) || call :buildFailLog vmime "build release" && goto clean_third_directory

xcopy %VMIME_DIR%\build%_X86%_debug\build\lib\Debug\*.lib %LIBS_DIR% /Y >nul 2>nul

xcopy %VMIME_DIR%\build%_X86%_debug\build\bin\Debug\*.dll %LIBS_DIR% /Y >nul 2>nul

cd ..

cd build%_X86%_release >nul 2>nul

( cmake -G %_CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE:STRING=Release ^
-DCMAKE_PREFIX_PATH="%TERRAMA_DEPENDENCIES_DIR%" ^
-DCMAKE_INSTALL_PREFIX="%TERRAMA_DEPENDENCIES_DIR%" ^
-DGNUTLS_INCLUDE_DIR="%INCLUDES_DIR%" -DGNUTLS_LIBRARY="%GNUTLS%" -DGSASL_LIBRARIES="%GSASL%" ^
-DVMIME_SENDMAIL_PATH="C:\VMime-SendMail" ^
 %VMIME_DIR% >>%CONFIG_LOG% 2>nul ) || call :buildFailLog vmime "configuring" && goto clean_third_directory

( msbuild /m /p:Configuration=Release vmime.vcxproj >>%BUILD_LOG% 2>nul ) || call :buildFailLog vmime "build release" && goto clean_third_directory

xcopy %VMIME_DIR%\build%_X86%_release\build\lib\Release\*.lib %LIBS_DIR% /Y >nul 2>nul

xcopy %VMIME_DIR%\build%_X86%_release\build\bin\Release\*.dll %LIBS_DIR% /Y >nul 2>nul

echo done.

call :append_log_end vmime

:end_vmime

cd %ROOT_DIR%
:: ====

:clean_third_directory

mkdir -p %TERRAMA_DEPENDENCIES_DIR%\bin >nul 2>nul 

xcopy bin %TERRAMA_DEPENDENCIES_DIR%\bin /S /Y >nul 2>nul

cd %TERRAMA_DEPENDENCIES_DIR% >nul 2>nul

RD /S /Q doc >nul 2>nul

RD /S /Q features >nul 2>nul

RD /S /Q plugins >nul 2>nul

del lib\*.exe /S /Q >nul 2>nul

del *.pdb /S /Q >nul 2>nul

IF DEFINED FAIL exit /b 1

cd %ROOT_DIR%
::exit /b 0