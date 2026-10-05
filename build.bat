@echo off
setlocal
time /t

set BUILD_DIR=build
set SOURCE_DIR=source
set INCLUDES=-I%SOURCE_DIR%\code -I%SOURCE_DIR% -I%SOURCE_DIR%\base
set CFLAGS=-FC -GR- -EHa- -nologo -Zi -FS /analyze /W3 /WX -MD /DYNAMICBASE:NO
set LIBS=user32.lib gdi32.lib kernel32.lib shell32.lib

if not exist %BUILD_DIR% mkdir %BUILD_DIR%

echo Building %BUILD_DIR%\program.exe...

cl %INCLUDES% %CFLAGS% %SOURCE_DIR%\code\compiler.cpp  /Fe%BUILD_DIR%\compiler.exe %LIBS%
