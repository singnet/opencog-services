FROM opencog/opencog-dev:cli

ENV SINGNET_INSTALL=/opt/singnet
ENV GOPATH=${SINGNET_INSTALL}/go
ENV PATH=$PATH:${SINGNET_INSTALL}/go/bin:/usr/lib/go-1.10/bin
ENV GUILE_AUTO_COMPILE=0
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/opencog:/usr/local/lib/opencog/modules

RUN apt update && \
    apt install -y apt-utils git

RUN mkdir -p $SINGNET_INSTALL

RUN cd ${SINGNET_INSTALL} && \
    git clone https://github.com/singnet/opencog-services.git && \
    cd opencog-services && \
    ./scripts/install_prerequisites.sh ${SINGNET_INSTALL}

RUN cd ${SINGNET_INSTALL}/opencog-services && \
    cp lib/libPatternMiner.so /usr/local/lib/opencog

RUN cd ${SINGNET_INSTALL}/opencog-services && \
    ./scripts/build.sh

WORKDIR ${SINGNET_INSTALL}/opencog-services

