FROM tomcat:8.0-jre8

# Port Bindind
EXPOSE 8080

# Customizable Variables

ENV GEOSERVER_URL "/geoserver"
ENV GEOSERVER_DATA_DIR /opt/geoserver/data_dir

# Environment Variables

RUN mkdir -p $GEOSERVER_DATA_DIR

# Configure TomCat. TODO: Export Catalina opts with Docker Arg to customize
RUN cd /usr/local/tomcat/bin && \
    echo "#!/bin/bash\n\
    export CATALINA_OPTS=\"$CATALINA_OPTS -server\"\n\
    export CATALINA_OPTS=\"$CATALINA_OPTS -Xms32768m\"\n\
    export CATALINA_OPTS=\"$CATALINA_OPTS -Xmx32768m\"\n" | tee -a setenv.sh && \
    chmod +x setenv.sh

# Install GeoServer
RUN wget --no-verbose "http://sourceforge.net/projects/geoserver/files/GeoServer/2.11.0/geoserver-2.11.0-war.zip" && \
    unzip geoserver-2.11.0-war.zip && \
    mv geoserver.war /root/ && \
    rm geoserver-2.11.0-war.zip

# Configure TomCat. Setting geoserver path
RUN sed "125i<Context path=\"$GEOSERVER_URL\" docBase=\"/root/geoserver.war\"/>" /usr/local/tomcat/conf/server.xml > /root/server.xml && \
    mv /root/server.xml /usr/local/tomcat/conf/server.xml
    
COPY docker-entrypoint.sh /

ENTRYPOINT [ "/docker-entrypoint.sh" ]
