#!/bin/bash

getent group docker
if [[ $? -ne 0 ]]; then
  USER_ID=`stat -c %g devel/`
  sudo groupadd -g ${USER_ID} docker
  sudo usermod -aG docker terrama2
fi 
