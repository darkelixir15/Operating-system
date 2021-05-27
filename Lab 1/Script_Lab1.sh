#!/bin/bash
rm -rf 9091
mkdir 9091
cd 9091
mkdir Fleischer
cd Kev
date > Kev
date –date ”next Monday” > filedate.txt
cat Kev filedate.txt > result.txt
cat result.txt
