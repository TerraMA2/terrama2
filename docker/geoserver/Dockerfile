FROM ubuntu:16.04

EXPOSE 8080

ENV GEOSERVER_HOME=/opt/geoserver

RUN apt-get update && \
    apt-get install -y openjdk-8-jdk \
                       curl \
                       unzip

# Install GeoServer
RUN cd /opt && \
    curl -LO https://ufpr.dl.sourceforge.net/project/geoserver/GeoServer/2.12.0/geoserver-2.12.0-bin.zip && \
    unzip -q geoserver-2.12.0-bin.zip && \
    mv geoserver-2.12.0 ${GEOSERVER_HOME} && \
    rm -rf geoserver-2.12.0-bin.zip

VOLUME /data

ENTRYPOINT ${GEOSERVER_HOME}/bin/startup.sh