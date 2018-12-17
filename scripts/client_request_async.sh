#!/bin/bash

SERVICE_JSON_PARAMS=$(cat << EOF
{"cmd":"$1","arg1":"$2","arg2":"$3","arg3":"$4","arg4":"$5","arg5":"$6"}
EOF
)


SERVICE_ENDPOINT=54.203.198.53:7031
export MPE_CHANNEL=`snet client print_all_channels | tail -1 | cut -d" " -f1`

echo "MPE channel id: $MPE_CHANNEL"
echo "Channel state before request..."
snet client get_channel_state $MPE_CHANNEL $SERVICE_ENDPOINT

snet client call $MPE_CHANNEL 0.00000001 $SERVICE_ENDPOINT asynchronousTask $SERVICE_JSON_PARAMS

echo "Channel state after request..."
snet client get_channel_state $MPE_CHANNEL $SERVICE_ENDPOINT
