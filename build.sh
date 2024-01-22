#!/bin/zsh

arduino-cli compile -b esp32:esp32:watchy

echo Compiled

if [ -z "$1" ]; then
    echo Missing FQBN, therefore no uploading
else
    arduino-cli upload -b esp32:esp32:watchy --board-options Revision=v20 -p $1
fi

echo Finished
