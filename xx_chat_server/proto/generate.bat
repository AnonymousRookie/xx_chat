set SRC_DIR=.\
set DST_DIR=.\gen

::C++
set CPP_DIR=..\src\base\protocol


if exist %DST_DIR% (
   rd /s %DST_DIR%
)

::C++
mkdir %DST_DIR%\cpp

protoc -I=%SRC_DIR% --cpp_out=%DST_DIR%\cpp\ %SRC_DIR%\im.base.proto
protoc -I=%SRC_DIR% --cpp_out=%DST_DIR%\cpp\ %SRC_DIR%\im.login.proto
protoc -I=%SRC_DIR% --cpp_out=%DST_DIR%\cpp\ %SRC_DIR%\im.message.proto
protoc -I=%SRC_DIR% --cpp_out=%DST_DIR%\cpp\ %SRC_DIR%\im.other.proto
protoc -I=%SRC_DIR% --cpp_out=%DST_DIR%\cpp\ %SRC_DIR%\im.server.proto

xcopy %DST_DIR%\cpp\* %CPP_DIR%\ /c/q/e
