#!/bin/sh

ORGANIZATION_NAME=snet
SERVICE_NAME=opencog

if [ -z "$1" ]
  then
    AMOUNT=0.0000001
  else
    AMOUNT=$1
fi

echo "Initial Balances"
snet client balance
snet client deposit --yes 0.0000001
snet client open_init_channel_registry --yes $ORGANIZATION_NAME $SERVICE_NAME $AMOUNT $((`snet client block_number` + 40320 + 500))
echo "Current Balances"
snet client balance
snet client print_all_channels
echo "Current block: `snet client block_number`"
