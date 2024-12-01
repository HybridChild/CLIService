#!/bin/bash

rm -rf out
mkdir out
mkdir out/build

cmake -S ./cliService -B out/build
