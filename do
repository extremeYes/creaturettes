#!/usr/bin/env bash

set -x

rm ./*.exe*
rm ./*.pdb

rm ./nob ./main

## Build && Run ##

cc -o nob src/nob.c

./nob
