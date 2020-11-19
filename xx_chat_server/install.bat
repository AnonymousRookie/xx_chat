@echo on

cd %~dp0
set Dir=%cd%

xcopy %Dir%\src\db_proxy_server\db_proxy_server.json %Dir%\build\run\Debug\ /s /Y
xcopy %Dir%\src\login_server\login_server.json %Dir%\build\run\Debug\ /s /Y
xcopy %Dir%\src\msg_server\msg_server.json %Dir%\build\run\Debug\ /s /Y
xcopy %Dir%\src\route_server\route_server.json %Dir%\build\run\Debug\ /s /Y


xcopy %Dir%\thirdparty\curl\lib\curl.dll %Dir%\build\run\Debug\ /s /Y
xcopy %Dir%\thirdparty\mariadb\lib\libmysql.dll %Dir%\build\run\Debug\ /s /Y

@echo off