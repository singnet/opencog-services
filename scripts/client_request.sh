#!/bin/bash

CHANNEL_ID=$1
SERVICE_JSON_PARAMS=$(cat << EOF
{"cmd":"$2","arg1":"$3","arg2":"$4","arg3":"$5","arg4":"$6","arg5":"$7"}
EOF
)


SERVICE_ENDPOINT=54.203.198.53:7031

echo "Channel state before request..."
snet client get_channel_state $CHANNEL_ID $SERVICE_ENDPOINT

snet client call $CHANNEL_ID 0.00000001 $SERVICE_ENDPOINT execute $SERVICE_JSON_PARAMS

echo "Channel state after request..."
snet client get_channel_state $CHANNEL_ID $SERVICE_ENDPOINT

