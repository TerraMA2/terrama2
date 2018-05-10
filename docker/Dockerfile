FROM ubuntu:16.04
ARG VERSION=4.0.5

ENV VERSION=${VERSION}
# install minimal dependencies
RUN  apt-get update \
  && apt-get install -y wget sudo git lsb-release

# install terralib
RUN mkdir -p devel/terralib && \
    cd devel/terralib && \
    wget http://www.dpi.inpe.br/terralib5/download/download.php?FileName=terralib-5.3.1-ubuntu-16.04.tar.gz && \
    mv download.php?FileName=terralib-5.3.1-ubuntu-16.04.tar.gz terralib-5.3.1-ubuntu-16.04.tar.gz && \
    tar zxvf terralib-5.3.1-ubuntu-16.04.tar.gz && \
    ./install.sh && \
    cd .. && \
    rm terralib -r

# terrama2 build dependencies and clone terrama2
RUN cd devel && \
    apt install -y cmake build-essential libcurl3-dev libpython2.7-dev libquazip-dev libxerces-c-dev libgeos++-dev libproj-dev && \
    git clone https://github.com/TerraMA2/terrama2 && \
    cd terrama2 && \
    git fetch && \
    git checkout b${VERSION}

# install vmime
RUN cd devel/terrama2/packages/debian-package && \
    sudo apt install -y doxygen graphviz gnutls-bin gsasl libghc-gsasl-dev libgnutls-dev libssl-dev debhelper devscripts  && \
    wget https://github.com/kisli/vmime/archive/v0.9.2.tar.gz && \
    ./deb-vmime.sh && \
    dpkg -i terrama2-vmime-${VERSION}_0.9.2-ubuntu16.04_amd64.deb || true && \
    apt update && \
    apt install -f -y

# clean apt
RUN rm -rf /var/lib/apt/lists/*

ADD ./entrypoint.sh /

WORKDIR /
ENTRYPOINT ./entrypoint.sh
