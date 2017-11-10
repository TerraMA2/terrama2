FROM ubuntu:14.04

############################################################################
# Information
LABEL maintainer="TerraMA² Team <terrama2-team@dpi.inpe.br>" \
      version="4.0.0"

############################################################################
# Port Binding

# PostgreSQL
EXPOSE 5432
# GeoServer
EXPOSE 8080
# TerraMA² WebApp
EXPOSE 36000
# TerraMA² WebMonitor
EXPOSE 36001

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
ENV PATH="$PATH:${TERRAMA2_INSTALL_PATH}/bin"

RUN echo "deb http://br.archive.ubuntu.com/ubuntu/ trusty-backports main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb-src http://br.archive.ubuntu.com/ubuntu/ trusty-backports main restricted universe multiverse" >> /etc/apt/sources.list && \
    apt-get update && \
    apt-get install -y cmake \
                       build-essential \
                       software-properties-common \
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
                       devscripts \
                       # TerraMA2 Service And Docker
                       dbus

# Creating USER
RUN groupadd $TERRAMA2_USER && \
    useradd $TERRAMA2_USER -s /bin/bash -m -g $TERRAMA2_USER && \
    # Defining User credential to connect
    echo $TERRAMA2_USER:$TERRAMA2_USER | chpasswd && \
    mkdir -p $TERRAMA2_3RDPARTY_PATH $TERRAMA2_CODEBASE_PATH && \
    chown -R $TERRAMA2_USER:$TERRAMA2_USER /home/$TERRAMA2_USER && \
    chmod g-w /home/$TERRAMA2_USER

############################################################################
# Dependencies

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

# NodeJS
RUN curl -sL https://deb.nodesource.com/setup_6.x | sudo -E bash - && \
    apt-get install -y nodejs

# VMime
RUN cd ${TERRAMA2_CODEBASE_PATH} && \
    curl -O http://www.dpi.inpe.br/jenkins-data/terrama2/3rdparty/terrama2-vmime_0.9.2-ubuntu-14.04_amd64.deb && \
    dpkg -i terrama2-vmime_0.9.2-ubuntu-14.04_amd64.deb && \
    apt-get install -f -y && \
    rm -rf terrama2-vmime_0.9.2-ubuntu-14.04_amd64.deb* v* ${TERRAMA2_CODEBASE_PATH}

# ############################################################################
# # TerraMA2
COPY files/install_terrama2.sh /usr/local/bin
COPY files/docker-entrypoint.sh /

RUN service postgresql start && \
    install_terrama2.sh

# ############################################################################
# # Volumes
VOLUME /data  

ENTRYPOINT /docker-entrypoint.sh