# About
This is a DXD12 Game Engine using an Xmake build system

# How to Set-Up
1. Go to Scripts Folder
3. Click the setup-windows bat file -- this is a bootstrap for xmake
4. Allow changes for the xmake
5. You will open a wizard set-up, Click on next
6. Click I Agree
7. Check Add to Path
8. Set a folder you want to install xmake
9. Install xmake
10. Click Finish
11. IDE project files for MSVC will be automatically generated
12. There will be a folder with the prefix "vs" following with the current MSVC version e.g. vs2026, vs2022. Select that folder and the .sln file is found there
13. Alternatively, there is a Generate.bat file which you can use after installing xmake and it will generate a build for an exe file that can be found on the bin/Debug-x64/App/App.exe.

# Entry Point
Entry Point is located in App/src/App.cpp
