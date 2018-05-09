# Docker
Simple instructions for using docker for TerraMA2 packages.

Obs. In this guide I assume you have permission to run `docker` as user, you may also run the commands using `sudo`

## Generate docker image (if you don't have one)
```
docker build -t "terrama2:4.0.5" .
docker run -td --name terrama2_4.0.5 -v ~/packages/4.0.5:/packages terrama2:4.0.5
```
## Start the docker image

A folder with the ubuntu version and current date will be created, in the container folder,
with the terrama2 packages.

```
docker container restart terrama2_4.0.5
```
