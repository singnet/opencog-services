#!/bin/bash

./bin/server &
snetd --config ./snetd_configs/snetd.mainnet.json &
snetd --config ./snetd_configs/snetd.ropsten.json &
