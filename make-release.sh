#!/bin/sh

./build.sh

mkdir -p release
cp output/jcc release/jcc

# Strip and compress binary
strip release/jcc
upx -9 release/jcc

rm -rf output
