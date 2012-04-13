#!/bin/bash
input=$1
if [[ $input == "" ]]; then
	exit
fi
gcc $(gsl-config --cflags) $input $(gsl-config --libs)
if [[ -e data.dat ]]; then
	rm data.dat
fi
if [[ -e data.ps ]]; then
	rm data.ps
fi
./a.out > data.dat
graph -T ps < data.dat > data.ps
evince data.ps
