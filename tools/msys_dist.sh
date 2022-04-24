#!/bin/bash

if [[ ! -f "libmbCHIP8.a" ]]; then
	echo "Run this script from the directory where you built libmbCHIP8."
	exit 1
fi

mkdir -p dist

if [ -d "mbCHIP8-SDL2" ]; then
	for lib in $(ldd mbCHIP8-SDL2/mbCHIP8-SDL2.exe | grep mingw | sed "s/.*=> //" | sed "s/(.*)//"); do
		cp "${lib}" dist
	done
	cp mbCHIP8-SDL2/mbCHIP8-SDL2.exe dist
fi