FROM ubuntu:16.04
ARG BRANCH=master
ARG REPOSITORY=TerraMA2

ENV BRANCH=${BRANCH}
ENV REPOSITORY=${REPOSITORY}
ENV DEPENDENCIES=/home/${USER}/devel/dependencies

# install minimal dependencies
RUN  apt-get update \
  && apt-get install -y wget sudo git lsb-release

# install terralib
RUN mkdir -p devel/terralib && \
    cd devel/terralib && \
    wget http://www.dpi.inpe.br/terralib5/download/download.php?FileName=terralib-5.3.3-ubuntu-16.04.tar.gz && \
    mv download.php?FileName=terralib-5.3.3-ubuntu-16.04.tar.gz terralib-5.3.3-ubuntu-16.04.tar.gz && \
    tar zxvf terralib-5.3.3-ubuntu-16.04.tar.gz && \
    ./install.sh && \
    cd .. && \
    rm terralib -r

# terrama2 build dependencies and clone terrama2
RUN cd devel && \
    apt install -y cmake build-essential libcurl3-dev libpython2.7-dev libxerces-c-dev libgeos++-dev libproj-dev && \
    git clone https://github.com/${REPOSITORY}/terrama2 && \
    cd terrama2 && \
    git fetch && \
    git checkout ${BRANCH}

# install vmime
RUN mkdir -p devel/dependencies && \
    cd devel/dependencies && \
    sudo apt install -y doxygen graphviz gnutls-bin gsasl libghc-gsasl-dev libgnutls-dev libssl-dev  && \
    wget https://github.com/kisli/vmime/archive/v0.9.2.tar.gz && \
    tar xzf v0.9.2.tar.gz && \
    cd vmime-0.9.2 && \
    cmake -G "Unix Makefiles" \
          -DCMAKE_BUILD_TYPE:STRING="Release" \
          -DVMIME_HAVE_MESSAGING_PROTO_SENDMAIL:BOOL=false \
          -DVMIME_BUILD_SAMPLES:BOOL=false \
          -DCMAKE_PREFIX_PATH:PATH="/opt/terralib/5.3.3/3rdparty" \
          -DCMAKE_INSTALL_PREFIX:PATH="${DEPENDENCIES}" && \
    make -j4 && \
    make install && \
    rm -rf v0.9.2* && \
    rm -rf vmime*

# install quazip
RUN mkdir -p devel/dependencies && \
    cd devel/dependencies && \
    sudo apt install -y zlib1g-dev  && \
    wget https://github.com/stachenov/quazip/archive/0.7.6.tar.gz && \
    tar xzf 0.7.6.tar.gz && \
    cd quazip-0.7.6 && \
    qmake "PREFIX=${DEPENDENCIES}" && \
    make -j4 && \
    make install && \
    rm -rf 0.7.6* && \
    rm -rf quazip*

# clean apt
RUN rm -rf /var/lib/apt/lists/*

ADD ./entrypoint.sh /

WORKDIR /
ENTRYPOINT ./entrypoint.sh
