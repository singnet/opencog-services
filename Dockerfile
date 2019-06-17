ARG BASIC_IMAGE=opencog_services_basic
FROM $BASIC_IMAGE

ENV SINGNET_INSTALL=/opt/singnet
ENV GOPATH=${SINGNET_INSTALL}/go
ENV PATH=$PATH:${SINGNET_INSTALL}/go/bin:/usr/lib/go-1.10/bin
ENV GUILE_AUTO_COMPILE=0
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/opencog:/usr/local/lib/opencog/modules
ENV RELEX_CONTAINER_NAME="172.18.0.2"
ENV OPENCOG_SERVER_PORT="7032"

RUN mkdir -p $SINGNET_INSTALL

COPY ./ ${SINGNET_INSTALL}/opencog-services 

RUN cd ${SINGNET_INSTALL}/opencog-services && \
    ./scripts/build.sh

WORKDIR ${SINGNET_INSTALL}/opencog-services
