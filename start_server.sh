#!/bin/bash

nohup ./pasteup -a -m=8 -l :4703 main > main/server.pasteup.4703.out 2>&1 &
nohup ./pasteup -a -m=8 -l :4803 news > news/server.pasteup.4803.out 2>&1 &
nohup ./pasteup -a -m=8 -l :4903 test > test/server.pasteup.4903.out 2>&1 &

nohup ./pasteup -m=8 -l :4704 main > main/server.pasteup.4704.out 2>&1 &
nohup ./pasteup -m=8 -l :4804 news > news/server.pasteup.4804.out 2>&1 &
nohup ./pasteup -m=8 -l :4904 test > test/server.pasteup.4904.out 2>&1 &

nohup ./pastedown -m=8 -l :4705 main > main/server.pastedown.4705.out 2>&1 &
nohup ./pastedown -m=8 -l :4805 news > news/server.pastedown.4805.out 2>&1 &
nohup ./pastedown -m=8 -l :4905 test > test/server.pastedown.4905.out 2>&1 &

nohup ./runbored main > main/runbored.out 2>&1 &
nohup ./runbored news > news/runbored.out 2>&1 &
nohup ./runbored test > test/runbored.out 2>&1 &
