#!/bin/bash

./bin/server &
snetd --config ./snetd_configs/snetd.mainnet.json &
