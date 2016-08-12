@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
cls
set EDITOR_FILES="%cd%\sources\windows_game_platform.cpp"
set EDITOR_LIBS="Kernel32.lib" "User32.lib" "Gdi32.lib" "Gdiplus.lib" "Dsound.lib" "Mfreadwrite.lib" "Ole32.lib" "Mfplat.lib"
set PLATFORM_COMMON_INCLUDES="%cd%\sources\windows_types.h"
pushd bin\x64
del *.pdb > NUL
call cl.exe /nologo /W0 /WX /Od /Zi /FS /Fdgame.pdb /Fegame.exe /FI %PLATFORM_COMMON_INCLUDES% %EDITOR_FILES% %EDITOR_LIBS% /link /SUBSYSTEM:WINDOWS /INCREMENTAL:NO
popd
