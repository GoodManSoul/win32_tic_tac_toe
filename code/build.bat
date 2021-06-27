
@echo off

mkdir ..\build
pushd ..\build


cl -Zi ..\code\win32_new_playground.cpp user32.lib Gdi32.lib winmm.lib

popd
