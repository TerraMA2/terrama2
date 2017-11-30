#!/bin/bash

# Updating geoserver path
sed "125c<Context path=\"$GEOSERVER_URL\" docBase=\"/root/geoserver.war\"/>" /usr/local/tomcat/conf/server.xml > /root/server.xml
mv /root/server.xml /usr/local/tomcat/conf/server.xml

catalina.sh run
