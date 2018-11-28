#!/bin/bash

source deployment_settings.sh

snet identity create $SERVICE_NAME key
snet service delete $ORGANIZATION_NAME $SERVICE_NAME
snet service metadata_init src/service_spec/ opencog $PUBLIC_KEY
snet service metadata_set_fixed_price 0.00000001
snet service metadata_add_endpoints http://$HOST_IP:$DAEMON_PORT
snet service publish --yes $ORGANIZATION_NAME $SERVICE_NAME
snet organization list-services $ORGANIZATION_NAME
