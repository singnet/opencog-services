#!/bin/bash

cd ./scripts
./start_daemon.sh &
cd ../bin
./server
