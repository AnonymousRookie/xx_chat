#!/bin/bash

if [ ! -d build ]; then
    mkdir build    
fi

cd build
cmake ../src/
make

cp -f ./base/libbase.a ./run/libbase.a
cp -f ./login_server/login_server ./run/login_server
cp -f ./msg_server/msg_server ./run/msg_server
cp -f ./db_proxy_server/db_proxy_server ./run/db_proxy_server
cp -f ./route_server/route_server ./run/route_server

cp -f ./base/libbase.a ../test/build