#!/bin/bash

./pastedown bored1.clipbored.io:4703 main > main/client.pastedown.4703.out 2>&1
./pastedown bored1.clipbored.io:4803 news > news/client.pastedown.4803.out 2>&1
./pastedown bored1.clipbored.io:4903 test > test/client.pastedown.4903.out 2>&1

nohup ./pastedown bored1.clipbored.io:4704 main > main/client.pastedown.4704.out 2>&1 &
nohup ./pastedown bored1.clipbored.io:4804 news > news/client.pastedown.4804.out 2>&1 &
nohup ./pastedown bored1.clipbored.io:4904 test > test/client.pastedown.4904.out 2>&1 &

nohup ./pasteup bored1.clipbored.io:4705 main > main/client.pasteup.4705.out 2>&1 &
nohup ./pasteup bored1.clipbored.io:4805 news > news/client.pasteup.4805.out 2>&1 &
nohup ./pasteup bored1.clipbored.io:4905 test > test/client.pasteup.4905.out 2>&1 &

nohup ./runbored main > main/runbored.out 2>&1 &
nohup ./runbored news > news/runbored.out 2>&1 &
nohup ./runbored test > test/runbored.out 2>&1 &
