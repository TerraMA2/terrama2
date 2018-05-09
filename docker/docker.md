# Docker
Simple instructions for using docker for TerraMA2 packages.

Obs. In this guide I assume you have permission to run `docker` as user, if you don't run the commands using `sudo`

## Generate docker image (if you don't have one)
```
docker build -t "terrama2:4.0.4" .
docker run -it terrama2:4.0.4
```
## Start the docker image
```
docker start 
```

## Access the docker shell
```
docker exec -it terrama2_4.0.4 bash
```

## Generate the package
```
cd packages/debian-package/
./deb-terrama2.sh
```

## Copy package from docker
```
docker cp terrama2_4.0.4:/devel/build-package/TerraMA2-4.0.4-release-linux-x64-Ubuntu-16.04.deb .
```
