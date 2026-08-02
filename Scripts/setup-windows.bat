@echo off

pushd ..
where xmake >nul 2>&1
if %errorlevel% == 0 (
    echo xmake already installed, skipping...
) else (

    start /wait "" Vendor\Binaries\Xmake\Windows\xmake-v3.0.5.win64.exe

)
git submodule update --init --recursive
xmake project -k vsxmake -m "debug;release"

popd
pause