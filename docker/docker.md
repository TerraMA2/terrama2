# Docker
Simple instructions for using docker for TerraMA2 packages.

Obs. In this guide I assume you have permission to run `docker` as user, if you don't run the commands using `sudo`

## Generate docker image (if you don't have one)
```
docker build -t "terrama2:4.0.5" .
docker run -it terrama2:4.0.5 --name terrama2_4.0.5
```
## Start the docker image
```
docker start terrama2_4.0.5
```

## Access the docker shell
```
docker exec -it terrama2_4.0.5 bash
```

## Generate the package
```
./entrypoint.sh
```

## Copy package from docker
```
docker cp terrama2_4.0.5:/packages/ .
```
