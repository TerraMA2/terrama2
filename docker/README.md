# Dockerfile for TerraMA²

Dockerfiles Repository for the TerraMA² Platform.

## Build and Run

To build the Docker image, install [Docker](https://docs.docker.com/engine/installation/) and [Docker Compose](https://docs.docker.com/compose/install/#prerequisites). After that, execute following commands:

```bash
cd /path/to/terrama2_codebase/docker
# Disable Access Control XHost
xhost +
# Build Docker Image
docker-compose build
# Start containers orchestration
docker-compose up
```

The Docker container will automatically create a network between containers and it will expose the following ports for each TerraMA² Service:

|   Application    | Port  |
|------------------|-------|
| TerraMA² WebApp  | 36000 |
| TerraMA² Monitor | 36001 |
| PostgreSQL       | 5432  |
| GeoServer        | 8080  |

Once everything is running, we must configure manually services addresses and ssh public keys.

Connect in `terrama2_webapp` container

```bash
docker exec -it terrama2_webapp bash
vi $TERRAMA2_INSTALL_PATH/webapp/config/instances/default.json
# Change host "127.0.0.1" to "terrama2_postgis" (Container name since Docker Compose creates a bridge network between containers and it will not be exposed)

# Generate SSH Public Key
ssh-keygen

# Copy own public key to TerraMA² Services Containers. Check TerraMA² Dockerfile.
ssh-copy-id -i ~/.ssh/id_rsa.pub terrama2@terrama2_alert
ssh-copy-id -i ~/.ssh/id_rsa.pub terrama2@terrama2_analysis
ssh-copy-id -i ~/.ssh/id_rsa.pub terrama2@terrama2_collector
ssh-copy-id -i ~/.ssh/id_rsa.pub terrama2@terrama2_view

exit
```

Connect in `terrama2_webmonitor` container

```bash
docker exec -it terrama2_webmonitor bash
vi $TERRAMA2_INSTALL_PATH/webmonitor/config/instances/default.json
# Change host "127.0.0.1" to an accessible host from web browser
exit
```

You also must configure TerraMA² Service through Web Application in `/adminstration/services`.

Open TerraMA² in browser and go to Services Page `/adminstration/services` and edit the following services:

- Select `Local Alert`:
  - Unmark `Local Service` and fill:
    - **Address** - **terrama2_alert**
    - **Port** - **22**
    - **User** - **terrama2**
  - In `Log`, change:
    - **Log hostname** - **terrama2_postgis**
  - Save

- Select `Local Analysis`:
  - Unmark `Local Service` and fill:
    - **Address** - **terrama2_analysis**
    - **Port** - **22**
    - **User** - **terrama2**
  - In `Log`, change:
    - **Log hostname** - **terrama2_postgis**
  - Save

- Select `Local Collector`:
  - Unmark `Local Service` and fill:
    - **Address** - **terrama2_collector**
    - **Port** - **22**
    - **User** - **terrama2**
  - In `Log`, change:
    - **Log hostname** - **terrama2_postgis**
  - Save

- Select `Local View`:
  - Unmark `Local Service` and fill:
    - **Address** - **terrama2_view**
    - **Port** - **22**
    - **User** - **terrama2**
  - In `Log`, change:
    - **Log hostname** - **terrama2_postgis**
  - In `Maps Server Parameters`, change:
    - **Address** - *An accesible host to GeoServer Container*
  - Save

## Notes

- The TerraMA² Service requires `-platform offscreen` argument to execute due issue with GUI display module.