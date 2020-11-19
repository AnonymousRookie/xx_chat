@echo on

cd %~dp0
set Dir=%cd%
set QtBinDir=D:\ProgramFiles\Qt\Qt5.9.9\5.9.9\msvc2015\bin

xcopy %Dir%\thirdparty\curl\lib\libcurl-d.dll %Dir%\Win32\Debug\ /s /Y
%QtBinDir%\windeployqt.exe %Dir%\Win32\Debug\xx_chat_client.exe


@echo off