@echo on

cd %~dp0\Debug\

set AppDir=%cd%

start %AppDir%\login_server.exe
start %AppDir%\route_server.exe
start %AppDir%\db_proxy_server.exe
start %AppDir%\msg_server.exe

@echo off