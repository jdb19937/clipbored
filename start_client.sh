#!/bin/bash

nohup ./pasteup bored1.clipbored.io:4704 main > main/client.pasteup.4704.out 2>&1 &
nohup ./pasteup bored1.clipbored.io:4804 news > news/client.pasteup.4804.out 2>&1 &
nohup ./pasteup bored1.clipbored.io:4904 test > test/client.pasteup.4904.out 2>&1 &

nohup ./pastedown bored1.clipbored.io:4705 main > main/client.pastedown.4705.out 2>&1 &
nohup ./pastedown bored1.clipbored.io:4805 news > news/client.pastedown.4805.out 2>&1 &
nohup ./pastedown bored1.clipbored.io:4905 test > test/client.pastedown.4905.out 2>&1 &
