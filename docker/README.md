# Dockerfile for TerraMA²

Dockerfiles Repository for the TerraMA² Platform.

## Build and Run

To build the Docker image, install [Docker Compose](https://docs.docker.com/compose/). After that, execute following commands:

```bash
xhost + # Disable Access Control XHost
docker-compose up
```

The Docker container will expose the following ports for each TerraMA² Service:

|   Application    | Port  |
|------------------|-------|
| TerraMA² WebApp  | 36000 |
| TerraMA² Monitor | 36000 |
| PostgreSQL       | 5432  |
| GeoServer        | 8080  |
