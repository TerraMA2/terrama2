# Dockerfile for TerraMA²

Dockerfiles Repository for the TerraMA² Platform.

## Build and Run

To build the Docker image, execute the follow commands:

```bash
cd /path/to/terrama2/docker
docker build --tag terrama2-4.0.0:14.04 .
```

Execute the generated TerraMA² image with following commands:

```bash
xhost +
docker run --name terrama2 \
           -v /path/to/volume/on/host:/data \
           # Mount X11 dir for XServer forwarding (Read-Only)
           -v /tmp/.X11-unix:/tmp/.X11-unix:ro \
           # Expose Display to container to work with XServer
           -e DISPLAY=$DISPLAY \
           # Port Bindinds
           -p 36000:36000 \
           -p 36001:36001 \
           -p 5432:5432 \
           -p 8080:8080 \
           -it terrama2:14.04
```

The Docker container will expose the following ports for each TerraMA² Service:

|   Application    | Port  |
|------------------|-------|
| TerraMA² WebApp  | 36000 |
| TerraMA² Monitor | 36000 |
| PostgreSQL       | 5432  |
| GeoServer        | 8080  |
