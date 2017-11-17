# Dockerfile for TerraMA²

Dockerfiles Repository for the TerraMA² Platform.

## Build and Run

To build the Docker image, install [Docker Compose](https://docs.docker.com/compose/install/#prerequisites). After that, execute following commands:

```bash
cd /path/to/terrama2_codebase/docker
# Disable Access Control XHost
xhost +
# Build Docker Image
docker-compose build
# Start containers orchestration
docker-compose up
```

The Docker container will automatically expose the following ports for each TerraMA² Service:

|   Application    | Port  |
|------------------|-------|
| TerraMA² WebApp  | 36000 |
| TerraMA² Monitor | 36000 |
| PostgreSQL       | 5432  |
| GeoServer        | 8080  |


**Note** that you must configure manually:

- PostgreSQL port (Use host as `terrama2_postgis`) in TerraMA² Web App (/webapp/config/instances/default.json)
- TerraMA² Web App in TerraMA² Web Monitor (Use host as `terrama2_webapp`) (/webmonitor/config/instances/default.json)
