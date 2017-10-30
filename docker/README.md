## Dockerfile for TerraMA² - Ubuntu 16.04

- **Dockerfile** - Instructions to build Docker image.
- **files**
  * **install_terrama2.sh** - Contains directives to install TerraMA²

  * **docker-entrypoint.sh** - Docker container entrypoint handler.

### Build and Run
To build the Docker image, execute the follow commands:

```bash
cd /path/to/terrama2/docker
docker build --tag terrama2:16.04 .
```

Execute the generated TerraMA² image with following commands:

```bash
# Create volume to store TerraMA2 data
docker volume create terrama2_data
# Run
docker run -v terrama2_data:/opt/terrama2/4.0.0 -p 36000:36000 -p 36001:36001 -p 5432:5432 -p 8080:8080 -it terrama2:16.04
```

The volume `terrama2_data` represents where data will be stored in real machine (You can even customize directory using `--opt device=/custom/path`. See more in [Docker Volume](https://docs.docker.com/engine/reference/commandline/volume_create)). The container port binding will expose the following criteria:


|   Application    | Port  |
|------------------|-------|
| TerraMA² WebApp  | 36000 |
| TerraMA² Monitor | 36000 |
| PostgreSQL       | 5432  |
| GeoServer        | 8080  |