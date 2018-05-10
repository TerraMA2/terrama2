# Docker
TerraMA2 development docker.

Obs. In this guide I assume you have permission to run `docker` as user, you may also run the commands using `sudo`

## Generate docker image (if you don't have one)

```
docker build -t "terrama2:dev" .
docker run -itd --name terrama2_dev -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY -v ~/MyDevel:/home/terrama2/devel terrama2:dev
```
## Start the docker image

```
docker container start terrama2_dev
```

# Run a software

The environment installed:
  - clang-6 (+tools)
  - gcc-8
  - gdb/lldb
  - valgrind
  - git

Visual tools:
  - cmake-gui
  - code (VSCode + cpp and cmake extensions)

```
docker exec -it terrama2_dev code
```
