#!/bin/bash

if [ ! -d "out" ]
then
	mkdir out
	mkdir out/build
fi

cmake -S ./code -B out/build
