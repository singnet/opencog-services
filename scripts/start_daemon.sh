#!/bin/bash

source deployment_settings.sh

cat > snetd.config.json << EOF
{
   "PRIVATE_KEY": "1000000000000000000000000000000000000000000000000000000000000000",
   "DAEMON_LISTENING_PORT": $DAEMON_PORT,
   "ETHEREUM_JSON_RPC_ENDPOINT": "https://kovan.infura.io",
   "PASSTHROUGH_ENABLED": true,
   "PASSTHROUGH_ENDPOINT": "http://localhost:$SERVICE_PORT"
   "REGISTRY_ADDRESS_KEY": "0x2e4b2f2b72402b9b2d6a7851e37c856c329afe38",
   "DAEMON_END_POINT": "http://$HOST_IP:$DAEMON_PORT",
   "IPFS_END_POINT": "http://ipfs.singularitynet.io:80",
   "ORGANIZATION_NAME": "$ORGANIZATION_NAME",
   "SERVICE_NAME": "$SERVICE_NAME",
   "LOG": {
   "LEVEL": "debug",
   "OUTPUT": {
       "TYPE": "stdout"
       }
   }
}
EOF
snetd
