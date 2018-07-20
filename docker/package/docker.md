# Docker
Simple instructions for using docker to create TerraMA2 packages.

Obs. In this guide I assume you have permission to run `docker` as user, you may also run the commands using `sudo`

## Generate docker image (if you don't have one)

The `BRANCH` parameter must be a valid, existing github branch. If none is provided 'master' is used.
The `REPOSITORY` where the code is stored, default terrama2. (https://github.com/REPOSITORY/terrama2)

```
docker build -t "terrama2:4.0.5" --build-arg BRANCH=b4.0.5 --build-arg REPOSITORY=terrama2 .
docker run -t --name terrama2_4.0.5 -v ~/packages:/packages terrama2:4.0.5
```
## Start the docker image

A folder with the ubuntu version and current date will be created, in the container folder,
with the terrama2 packages.

```
docker container start terrama2_4.0.5
```
