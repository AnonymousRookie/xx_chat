#!/bin/sh

SRC_DIR=./
DST_DIR=./gen

#C++
CPP_DIR=../src/base/protocol

rm -rf $DST_DIR
rm -rf $CPP_DIR

#C++
mkdir -p $DST_DIR/cpp
protoc -I=$SRC_DIR --cpp_out=$DST_DIR/cpp/ $SRC_DIR/*.proto

mkdir $CPP_DIR
cp $DST_DIR/cpp/* $CPP_DIR/
