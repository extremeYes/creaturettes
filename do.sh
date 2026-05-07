#!/usr/bin/env bash

PS4=$'\e[31m$ \e[m'
set -x

## Remove Binaries ##

rm ./*.exe*
rm ./*.pdb

rm ./nob ./main

rm -r bin/*

## Build && Run ##

set -e

mkdir -p bin
mkdir -p ignored

cc -o nob src/nob.c

./nob
