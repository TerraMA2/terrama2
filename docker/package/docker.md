# Docker
Simple instructions for using docker to create TerraMA2 packages.

Obs. In this guide I assume you have permission to run `docker` as user, you may also run the commands using `sudo`

## Generate docker image (if you don't have one)

The `VERSION` parameter must be in the format `X.Y.Z` and have an associated branch with name in the format `bX.Y.Z`.

```
docker build -t "terrama2:4.0.5" --build-arg VERSION=4.0.5 .
docker run -t --name terrama2_4.0.5 -v ~/packages:/packages terrama2:4.0.5
```
## Start the docker image

A folder with the ubuntu version and current date will be created, in the container folder,
with the terrama2 packages.

```
docker container start terrama2_4.0.5
```
