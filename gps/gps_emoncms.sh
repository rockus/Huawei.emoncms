#!/bin/bash

HOST="emon.rockus.net"
NODE="time"
APIKEY="4f0162ae3f3e67c84336950ad1b98abe"

while true; do
  FIX=`gpspipe -rn12 | awk -F',' '/GPGSA/ {print $3}'`
  curl "$HOST/input/post?node=$NODE&json={GPS_fix:$FIX}&apikey=$APIKEY" >/dev/null 2>&1
done
