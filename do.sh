#!/usr/bin/env bash

{
    set -x

    rm ./*.exe*
    rm ./*.pdb

    rm ./nob ./main
} 2>/dev/null

## Build && Run ##

set -xe

cc -o nob src/nob.c

./nob
