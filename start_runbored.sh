#!/bin/bash

nohup ./runbored main > main/runbored.out 2>&1 &
nohup ./runbored news > news/runbored.out 2>&1 &
nohup ./runbored test > test/runbored.out 2>&1 &
