#!/bin/bash

SERVER_URI='http://www.terrama2.dpi.inpe.br/pub/downloads/terrama2q/'
TAR_GZ_FILENAME='terrama2q.tar.gz'
TEMP_FOLDER='/tmp/terrama2q'

DB_HOST='localhost'
DB_PORT='5432'
DB_NAME='terrama2qteste'
DB_USERNAME='postgres'

FILE_NAME_SCHEMA_BIOMAS='terrama2q_biomas_schema.sql'
FILE_NAME_DATA_BIOMAS='terrama2q_biomas_data.sql'
FILE_NAME_POST_BIOMAS='terrama2q_biomas_post.sql'

FILE_NAME_SCHEMA_UCFS='terrama2q_ucfs_schema.sql'
FILE_NAME_DATA_UCFS='terrama2q_ucfs_data.sql'
FILE_NAME_POST_UCFS='terrama2q_ucfs_post.sql'

FILE_NAME_SCHEMA_PAISES='terrama2q_paises_schema.sql'
FILE_NAME_DATA_PAISES='terrama2q_paises_data.sql'
FILE_NAME_POST_PAISES='terrama2q_paises_post.sql'

FILE_NAME_SCHEMA_ESTADOS='terrama2q_estados_schema.sql'
FILE_NAME_DATA_ESTADOS='terrama2q_estados_data.sql'
FILE_NAME_POST_ESTADOS='terrama2q_estados_post.sql'

FILE_NAME_SCHEMA_MUNICIPIOS='terrama2q_municipios_schema.sql'
FILE_NAME_DATA_MUNICIPIOS='terrama2q_municipios_data.sql'
FILE_NAME_POST_MUNICIPIOS='terrama2q_municipios_post.sql'

FILE_NAME_SCHEMA_FOCOS='terrama2q_focos_schema.sql'
FILE_NAME_DATA_FOCOS='terrama2q_focos_data.sql'
FILE_NAME_POST_FOCOS='terrama2q_focos_post.sql'
FILE_NAME_VIEW_FOCOS='terrama2q_focos_view.sql'

echo ""
echo $(date)
echo ""
echo "####################################"
echo "# Starting restore..."
echo "####################################"
echo ""

# Clear and create a temporary folder
echo "# Creating temporary folder..."
echo ""

rm -rf ${TEMP_FOLDER}
mkdir -p ${TEMP_FOLDER}

(
  cd ${TEMP_FOLDER}

  echo "# Downloading ${SERVER_URI}/${TAR_GZ_FILENAME}..."
  echo ""
  wget -q ${SERVER_URI}/${TAR_GZ_FILENAME}

  # Decompress the dump to the temporary folder
  echo "# Decompressing ${TAR_GZ_FILENAME}..."
  echo ""
  tar xzf ${TAR_GZ_FILENAME}

  # Create the database
  echo "# Creating database ${DB_NAME}..."
  echo ""
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -U "${DB_USERNAME}" -c "create database ${DB_NAME};"

  # Create extension 'postgis'
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -c "CREATE EXTENSION postgis;"
)

echo "# Restoring TerraMA2Q database..."
echo ""

# Biomas
(
  cd ${TEMP_FOLDER}

  echo "# Restoring table 'biomas'..."
  echo ""
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_SCHEMA_BIOMAS}
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_DATA_BIOMAS}
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_POST_BIOMAS}
)

# Unidades de Conservação Federais
(
  cd ${TEMP_FOLDER}

  echo "# Restoring table 'unidades_conservacao_federais'..."
  echo ""
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_SCHEMA_UCFS}
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_DATA_UCFS}
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_POST_UCFS}
)

# Países
(
  cd ${TEMP_FOLDER}

  echo "# Restoring table 'paises_light'..."
  echo ""
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_SCHEMA_PAISES}
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_DATA_PAISES}
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_POST_PAISES}
)

# Estados
(
  cd ${TEMP_FOLDER}

  echo "# Restoring table 'estados_light'..."
  echo ""
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_SCHEMA_ESTADOS}
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_DATA_ESTADOS}
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_POST_ESTADOS}
)

# Municípios
(
  cd ${TEMP_FOLDER}

  echo "# Restoring table 'municipios_light'..."
  echo ""
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_SCHEMA_MUNICIPIOS}
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_DATA_MUNICIPIOS}
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_POST_MUNICIPIOS}
)

# Focos
(
  echo "# Restoring table 'focos_terrama2q'..."
  echo ""
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_SCHEMA_FOCOS}
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_DATA_FOCOS}
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_POST_FOCOS}
  psql -W -q -h ${DB_HOST} -p ${DB_PORT} -d ${DB_NAME} -U "${DB_USERNAME}" -f ${FILE_NAME_VIEW_FOCOS}
)

rm -rf ${TEMP_FOLDER}

echo "####################################"
echo "# Restore finished!"
echo "####################################"
echo ""
echo $(date)
echo ""

exit 0
