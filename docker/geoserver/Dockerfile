FROM ubuntu:16.04

# Port Bindind
EXPOSE 8080

# Customizable Variables

# Environment Variables


RUN apt-get update && \
    apt-get install -y software-properties-common wget unzip nano && \
    add-apt-repository ppa:webupd8team/java && \
    apt-get update && \
    yes | apt-get install -y oracle-java8-installer && \
    yes | apt-get install -y oracle-java8-set-default && \
    # Installing Tomcat
    apt-get install -y tomcat8 tomcat8-admin

# Configure TomCat. TODO: Export Catalina opts with Docker Arg to customize
RUN cd /usr/share/tomcat8/bin && \
    echo "#!/bin/bash\n\
    export CATALINA_OPTS=\"$CATALINA_OPTS -server\"\n\
    export CATALINA_OPTS=\"$CATALINA_OPTS -Xms32768m\"\n\
    export CATALINA_OPTS=\"$CATALINA_OPTS -Xmx32768m\"\n\
    JAVA_HOME=\"/usr/lib/jvm/java-8-oracle\"\n" | tee -a setenv.sh && \
    chmod +x setenv.sh

# Install GeoServer
RUN wget "http://sourceforge.net/projects/geoserver/files/GeoServer/2.11.0/geoserver-2.11.0-war.zip" && \
    unzip geoserver-2.11.0-war.zip && \
    mv geoserver.war /var/lib/tomcat8/webapps/ && \
    rm geoserver-2.11.0-war.zip

COPY docker-entrypoint.sh /

ENTRYPOINT [ "/docker-entrypoint.sh" ]