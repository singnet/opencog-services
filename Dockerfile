FROM senna_opencog_services_basic

ENV SINGNET_INSTALL=/opt/singnet
ENV GOPATH=${SINGNET_INSTALL}/go
ENV PATH=$PATH:${SINGNET_INSTALL}/go/bin:/usr/lib/go-1.10/bin
ENV GUILE_AUTO_COMPILE=0
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/opencog:/usr/local/lib/opencog/modules

RUN mkdir -p $SINGNET_INSTALL

RUN cd ${SINGNET_INSTALL} && \
    git clone https://github.com/singnet/opencog-services.git

RUN cd ${SINGNET_INSTALL}/opencog-services && \
    ./scripts/build.sh

WORKDIR ${SINGNET_INSTALL}/opencog-services
