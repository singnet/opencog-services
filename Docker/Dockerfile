FROM opencog/opencog-dev:cli
ENV GOPATH=/opt/singnet/go
ENV PATH=$PATH:/opt/singnet/go/bin:/usr/lib/go-1.10/bin
RUN apt update && apt install -y apt-utils git
RUN mkdir -p /opt/singnet
ADD https://raw.githubusercontent.com/singnet/opencog-services/master/scripts/install_prerequisites.sh /opt/singnet
RUN chmod 777 /opt/singnet/install_prerequisites.sh
RUN cd /opt/singnet && ./install_prerequisites.sh /opt/singnet 
ENV SINGNET_INSTALL=/opt/singnet
ENV GUILE_AUTO_COMPILE=0
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/opencog:/usr/local/lib/opencog/modules

RUN mkdir -p $SINGNET_INSTALL

RUN cd ${SINGNET_INSTALL} && \
    git clone https://github.com/singnet/opencog-services.git

RUN cd ${SINGNET_INSTALL}/opencog-services && \
    cp lib/libPatternMiner.so /usr/local/lib/opencog

RUN cd ${SINGNET_INSTALL}/opencog-services && \
    ./scripts/build.sh

WORKDIR ${SINGNET_INSTALL}/opencog-services
