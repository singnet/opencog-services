#!/bin/sh

TMP_FILE=/tmp/__SNET_SERVICE_PUBLISH_LOG.txt
rm -f $TMP_FILE
yes | snet service publish 2>&1 | tee $TMP_FILE
./bin/server &
AGENT_ADDRESS=$(cat $TMP_FILE | grep current_agent_at | cut -d" " -f3)
sed -i "s/__AGENT_ADDRESS__/$AGENT_ADDRESS/g" config/snetd.config.json
sed -i "s/__AGENT_ADDRESS__/$AGENT_ADDRESS/g" testServiceRequest.sh
sed -i "s/__PRIVATE_KEY__/$1/g" config/snetd.config.json
snetd --config config/snetd.config.json &
sleep 3
