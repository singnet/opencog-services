#!/bin/bash

PORT=7031
ORGANIZATION_NAME=snet
SERVICE_NAME=opencog
PUBLIC_KEY=0x05F8aea226E4E331146B93AB12A6aa1a48B012a4

snet identity create $SERVICE_NAME key
snet service metadata_init src/service_spec/ opencog $PUBLIC_KEY
snet service metadata_set_fixed_price 0.00000001
snet service metadata_add_endpoints http://54.203.198.53:$PORT
snet service publish --yes $ORGANIZATION_NAME $SERVICE_NAME
snet organization list-services $ORGANIZATION_NAME
cat > snetd.config.json << EOF
{
   "PRIVATE_KEY": "1000000000000000000000000000000000000000000000000000000000000000",
   "DAEMON_LISTENING_PORT": $PORT,
   "ETHEREUM_JSON_RPC_ENDPOINT": "https://kovan.infura.io",
   "PASSTHROUGH_ENABLED": true,
   "PASSTHROUGH_ENDPOINT": "http://localhost:7003",
   "REGISTRY_ADDRESS_KEY": "0x2e4b2f2b72402b9b2d6a7851e37c856c329afe38",
   "DAEMON_END_POINT": "http://54.203.198.53:$PORT",
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
snetd &
./bin/server &
