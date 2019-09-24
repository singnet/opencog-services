#!/bin/bash

# configuration
PASSTHROUGH_ENDPOINT=7055
OC_SERVICES_PORT_MAINNET=7095
RELEX_PORT=7094

# remove old images and containers
docker rm -f opencog_services_relex_container
docker rm -f opencog_services_container
docker rmi opencog_services_relex_image
docker rmi opencog_services_image

# build dependencies
docker build -t opencog_services_relex_image -f OpenCogRelex .
docker build -t opencog_services_image -f OpenCogServices .

# setup network
docker network rm opencog_services_network
docker network create opencog_services_network

# run containers
docker run --name opencog_services_relex_container \
           --restart unless-stopped \
           --network opencog_services_network \
           -p $RELEX_PORT:4444 \
           -di opencog_services_relex_image tail -f /dev/null

docker run --name opencog_services_container \
           --env RELEX_CONTAINER_NAME=opencog_services_relex_container \
           --env OPENCOG_SERVER_PORT=$PASSTHROUGH_ENDPOINT \
           --restart unless-stopped \
           --network opencog_services_network \
           -v ~/singnet/.certs:/opt/singnet/.certs \
           -p $OC_SERVICES_PORT_MAINNET:$OC_SERVICES_PORT_MAINNET \
           -di opencog_services_image tail -f /dev/null

# start service
docker exec -d opencog_services_relex_container bash opencog-server.sh
docker exec -d opencog_services_container ./bin/server
docker exec -d opencog_services_container snetd --config ./snetd_configs/snetd.mainnet.json
