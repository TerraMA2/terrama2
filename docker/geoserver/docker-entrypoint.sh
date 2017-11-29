#!/bin/bash

service tomcat8 start

# Exposing Tomcat Log to Lock Container Execution
tail -f /var/log/tomcat8/catalina.out