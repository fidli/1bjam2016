@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
cls
set GAME_FILES="%cd%\sources\windows32_game.cpp"
set GAME_LIBS="Kernel32.lib" "User32.lib" "Gdi32.lib" "Gdiplus.lib" "Dsound.lib" "Mfplat.lib" "Ole32.lib" "Mfreadwrite.lib" "Mfuuid.lib" "Advapi32.lib" "Shell32.lib"
pushd build
del *.pdb
call cl.exe /nologo /W0 /WX /EHa- /GS- /GR- /Od /Zi /FS /Fdgame32.pdb /Fegame32.exe %GAME_FILES%  /link /INCREMENTAL:NO /NODEFAULTLIB:libcmt.lib /SUBSYSTEM:WINDOWS %GAME_LIBS%
popd
