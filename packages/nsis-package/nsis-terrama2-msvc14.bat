:: Turn off system messages
@echo off
:: Clear screen
cls;
:: Welcome message
echo.
echo **************************************************
echo * TerraMa2 Package Installer for Windows 64-bits *
echo **************************************************
echo.
:: ====

:: Where is script file
set "ROOT_DIR=%CD%"

:: TerraMa2 Paths
set "TERRALIB_DEPENDENCIES_DIR=C:\terralib5-3rdparty-msvc-2015-win64"
set "TERRAMA2_DEPENDENCIES_DIR=C:\terrama2-3rdparty-msvc-2015-win64"
set "TERRAMA2_CODEBASE_PATH=%ROOT_DIR%\..\.."
set "TERRAMA2_BUILD_PATH=%TERRAMA2_CODEBASE_PATH%\..\build-package-b4.0"
set "TERRAMA2_INSTALL_PATH=%TERRAMA2_DEPENDENCIES_DIR%\terrama2-install-b4.0"
set "TERRALIB_INSTALL_DIR=%TERRALIB_DEPENDENCIES_DIR%\terralib-install\lib\cmake\terralib-5.3.0"

:: Where is qmake.exe
set "QMAKE_FILEPATH=C:\Qt\Qt5.8.0\5.8\msvc2015_64\bin"
:: Where is Visual Studio
set "VCVARS_FILEPATH=%PROGRAMFILES(x86)%\Microsoft Visual Studio 14.0\VC"
:: Where is cmake.exe
set "CMAKE_FILEPATH=%PROGRAMFILES%\CMake\bin"

:: Environment Paths
set "PATH=%CMAKE_FILEPATH%;%QMAKE_FILEPATH%;%PATH%"

::  Setting visual studio environment
::  ==================================

set _CMAKE_GENERATOR="Visual Studio 14 2015 Win64"

echo | set /p="Configuring visual studio... "<nul

call "%VCVARS_FILEPATH%"\vcvarsall.bat %_config%

echo done.
echo.

::  Creating and configuring build folder
::  ======================================

echo | set /p="Configuring build folder... "<nul

IF NOT EXIST %TERRAMA2_BUILD_PATH% (
  mkdir %TERRAMA2_BUILD_PATH% >nul 2>nul
)

xcopy terrama2.conf.cmake %TERRAMA2_BUILD_PATH% /S /Y >nul 2>nul

echo done.
echo.

IF EXIST %TERRAMA2_BUILD_PATH% (
  cd /d %TERRAMA2_BUILD_PATH% >nul 2>nul
)

::  Configuring CMake and generating files
::  =======================================

echo | set /p="Configuring CMake... "<nul

cmake -G %_CMAKE_GENERATOR% -C ./terrama2.conf.cmake -DCMAKE_BUILD_TYPE="Release" %TERRAMA2_CODEBASE_PATH%\build\cmake

echo done.
echo.

::  Building Visual Studio Projects
::  ================================

echo | set /p="Building project... "<nul

msbuild /m /t:clean ALL_BUILD.vcxproj /p:Configuration=Release

msbuild /m ALL_BUILD.vcxproj /p:Configuration=Release

echo done.
echo.

::  Installing project in dependencies dir
::  =======================================

echo | set /p="Installing... "<nul

msbuild /m INSTALL.vcxproj /p:Configuration=Release

echo done.
echo.

::  Generating TerraLib NSIS package
::  =================================

echo | set /p="Packaging... "<nul

msbuild /m PACKAGE.vcxproj /p:Configuration=Release

echo done.
echo.

:: ====

echo.
echo.
echo ************************************************************
echo * TerraMa2 Package Installer for Windows 64-bits finished! *
echo ************************************************************
echo Finished!
echo.

exit /b 0
