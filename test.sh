#!/bin/bash

clear
make clean
make

Xephyr :1 -screen 1024x576 &
until DISPLAY=:1 xdpyinfo >/dev/null 2>&1; do
    sleep 0.1
done
DISPLAY=:1 ./shorkwm &
sleep 0.3
DISPLAY=:1 xterm &
DISPLAY=:1 galculator &
wait
