FROM ubuntu:14.04

VOLUME /opt/terrama2/4.0.0
VOLUME /opt/geoserver

############################################################################
# Customizable Variables


############################################################################
# Environment Variables
ENV TERRAMA2_USER=terrama2
ENV DEV_DIR=/home/${TERRAMA2_USER}/mydevel
ENV TERRAMA2_INSTALL_PATH=/opt/terrama2/4.0.0
ENV TERRAMA2_CODEBASE_PATH=${DEV_DIR}/terrama2/codebase
ENV TERRAMA2_3RDPARTY_PATH=${DEV_DIR}/terrama2/3rdparty
ENV GEOSERVER_HOME=/opt/geoserver

RUN apt-get update && \
    apt-get install -y cmake \
                       build-essential \
                       libcurl3-dev \
                       libpython2.7-dev \
                       libquazip0-dev \
                       libproj-dev \
                       libgeos++-dev \
                       libxerces-c-dev \
                       git \
                       postgresql-9.3-postgis-2.1 \
                       unzip \
                       sudo \
                       screen \
                       # VMime Dependencies
                       doxygen \
                       graphviz \
                       gnutls-bin \
                       gsasl \
                       libghc-gsasl-dev \
                       libgnutls-dev \
                       libssl-dev \
                       debhelper \
                       devscripts

# Creating USER
RUN groupadd $TERRAMA2_USER && \
    useradd $TERRAMA2_USER -s /bin/bash -m -g $TERRAMA2_USER && \
    # Defining User credential to connect
    echo $TERRAMA2_USER:$TERRAMA2_USER | chpasswd && \
    mkdir -p $TERRAMA2_3RDPARTY_PATH $TERRAMA2_CODEBASE_PATH \
    && chown -R $TERRAMA2_USER:$TERRAMA2_USER /home/$TERRAMA2_USER \
    && chmod g-w /home/$TERRAMA2_USER

############################################################################
# Dependencies

# VMime
RUN cd ${TERRAMA2_CODEBASE_PATH} && \
    git clone https://github.com/terrama2/terrama2.git ${TERRAMA2_CODEBASE_PATH} && \
    cd packages/debian-package && \
    curl -LO https://github.com/kisli/vmime/archive/v0.9.2.tar.gz && \
    ./deb-vmime.sh && \
    dpkg -i terrama2-vmime_0.9.2-ubuntu14.04_amd64.deb && \
    apt-get install -f -y
    # rm -rf terrama2-vmime_0.9.2-ubuntu14.04_amd64* v*

# NodeJS
RUN curl -sL https://deb.nodesource.com/setup_6.x | sudo -E bash - && \
    apt-get install -y nodejs

# TerraLib
RUN cd ${TERRAMA2_3RDPARTY_PATH} && \
    curl -o terralib-5.2.3-ubuntu-14.04.tar.gz -L http://www.dpi.inpe.br/terralib5/download/download.php?FileName=terralib-5.2.3-ubuntu-14.04.tar.gz && \
    tar zxf terralib-5.2.3-ubuntu-14.04.tar.gz && \
    ./install.sh && \
    rm -rf *

# GeoServer
RUN cd ${TERRAMA2_3RDPARTY_PATH} && \
    # Dependencies
    add-apt-repository ppa:openjdk-r/ppa -y && \
    apt-get update && \
    apt-get install -y openjdk-8-jdk && \
    # Install GeoServer
    curl -LO https://ufpr.dl.sourceforge.net/project/geoserver/GeoServer/2.12.0/geoserver-2.12.0-bin.zip && \
    unzip -q geoserver-2.12.0-bin.zip && \
    mv geoserver-2.12.0 ${GEOSERVER_HOME} && \
    rm -rf geoserver-2.12.0-bin.zip

############################################################################
# TerraMA2
COPY files/install_terrama2.sh /usr/local/bin
COPY files/docker-entrypoint.sh /

RUN service postgresql start && \
    install_terrama2.sh

ENTRYPOINT /docker-entrypoint.sh