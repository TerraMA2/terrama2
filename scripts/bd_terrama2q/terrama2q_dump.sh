#!/bin/bash

HOST='000.000.000.000'
PORT='5432'
USERNAME='postgres'
DATABASE='terrama2q'
TMP_FOLDER='tmp_data_terrama2q'
TAR_GZ_FILENAME='terrama2q.tar.gz'

TABLE_NAME_BIOMAS='public.biomas'
FILE_NAME_SCHEMA_BIOMAS='terrama2q_biomas_schema.sql'
FILE_NAME_DATA_BIOMAS='terrama2q_biomas_data.sql'
FILE_NAME_POST_BIOMAS='terrama2q_biomas_post.sql'

TABLE_NAME_UCFS='public.unidades_conservacao_federais'
FILE_NAME_SCHEMA_UCFS='terrama2q_ucfs_schema.sql'
FILE_NAME_DATA_UCFS='terrama2q_ucfs_data.sql'
FILE_NAME_POST_UCFS='terrama2q_ucfs_post.sql'

TABLE_NAME_PAISES='public.paises_light'
FILE_NAME_SCHEMA_PAISES='terrama2q_paises_schema.sql'
FILE_NAME_DATA_PAISES='terrama2q_paises_data.sql'
FILE_NAME_POST_PAISES='terrama2q_paises_post.sql'

TABLE_NAME_ESTADOS='public.estados_light'
FILE_NAME_SCHEMA_ESTADOS='terrama2q_estados_schema.sql'
FILE_NAME_DATA_ESTADOS='terrama2q_estados_data.sql'
FILE_NAME_POST_ESTADOS='terrama2q_estados_post.sql'

TABLE_NAME_MUNICIPIOS='public.municipios_light'
FILE_NAME_SCHEMA_MUNICIPIOS='terrama2q_municipios_schema.sql'
FILE_NAME_DATA_MUNICIPIOS='terrama2q_municipios_data.sql'
FILE_NAME_POST_MUNICIPIOS='terrama2q_municipios_post.sql'

TABLE_NAME_FOCOS='public.focos_terrama2q'
VIEW_NAME_FOCOS='public.focos_bdq_light'
FILE_NAME_SCHEMA_FOCOS='terrama2q_focos_schema.sql'
FILE_NAME_DATA_FOCOS='terrama2q_focos_data.sql'
FILE_NAME_DATA_FOCOS_TMP='terrama2q_focos_data_tmp.sql'
FILE_NAME_POST_FOCOS='terrama2q_focos_post.sql'
FILE_NAME_VIEW_FOCOS='terrama2q_focos_view.sql'

echo ""
echo $(date)
echo ""
echo "####################################"
echo "# Starting dump..."
echo "####################################"
echo ""

mkdir ${TMP_FOLDER}
cd ${TMP_FOLDER}

# Biomas
(
  echo "# Dumping Biomas..."
  echo ""

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --clean --section pre-data --schema-only --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_SCHEMA_BIOMAS}" --table "${TABLE_NAME_BIOMAS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_SCHEMA_BIOMAS}
  echo "commit;" >> ${FILE_NAME_SCHEMA_BIOMAS}

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --section data --column-inserts --data-only --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_DATA_BIOMAS}" --table "${TABLE_NAME_BIOMAS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_DATA_BIOMAS}
  echo "commit;" >> ${FILE_NAME_DATA_BIOMAS}

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --section post-data --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_POST_BIOMAS}" --table "${TABLE_NAME_BIOMAS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_POST_BIOMAS}
  echo "commit;" >> ${FILE_NAME_POST_BIOMAS}
)

# Unidades de Conservação Federais
(
  echo "# Dumping Unidades de Conservação Federais..."
  echo ""

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --clean --section pre-data --schema-only --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_SCHEMA_UCFS}" --table "${TABLE_NAME_UCFS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_SCHEMA_UCFS}
  echo "commit;" >> ${FILE_NAME_SCHEMA_UCFS}

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --section data --column-inserts --data-only --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_DATA_UCFS}" --table "${TABLE_NAME_UCFS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_DATA_UCFS}
  echo "commit;" >> ${FILE_NAME_DATA_UCFS}

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --section post-data --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_POST_UCFS}" --table "${TABLE_NAME_UCFS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_POST_UCFS}
  echo "commit;" >> ${FILE_NAME_POST_UCFS}
)

# Países
(
  echo "# Dumping Países..."
  echo ""

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --clean --section pre-data --schema-only --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_SCHEMA_PAISES}" --table "${TABLE_NAME_PAISES}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_SCHEMA_PAISES}
  echo "commit;" >> ${FILE_NAME_SCHEMA_PAISES}

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --section data --column-inserts --data-only --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_DATA_PAISES}" --table "${TABLE_NAME_PAISES}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_DATA_PAISES}
  echo "commit;" >> ${FILE_NAME_DATA_PAISES}

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --section post-data --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_POST_PAISES}" --table "${TABLE_NAME_PAISES}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_POST_PAISES}
  echo "commit;" >> ${FILE_NAME_POST_PAISES}
)

# Estados
(
  echo "# Dumping Estados..."
  echo ""

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --clean --section pre-data --schema-only --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_SCHEMA_ESTADOS}" --table "${TABLE_NAME_ESTADOS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_SCHEMA_ESTADOS}
  echo "commit;" >> ${FILE_NAME_SCHEMA_ESTADOS}

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --section data --column-inserts --data-only --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_DATA_ESTADOS}" --table "${TABLE_NAME_ESTADOS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_DATA_ESTADOS}
  echo "commit;" >> ${FILE_NAME_DATA_ESTADOS}

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --section post-data --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_POST_ESTADOS}" --table "${TABLE_NAME_ESTADOS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_POST_ESTADOS}
  echo "commit;" >> ${FILE_NAME_POST_ESTADOS}
)

# Municípios
(
  echo "# Dumping Municípios..."
  echo ""

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --clean --section pre-data --schema-only --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_SCHEMA_MUNICIPIOS}" --table "${TABLE_NAME_MUNICIPIOS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_SCHEMA_MUNICIPIOS}
  echo "commit;" >> ${FILE_NAME_SCHEMA_MUNICIPIOS}

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --section data --column-inserts --data-only --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_DATA_MUNICIPIOS}" --table "${TABLE_NAME_MUNICIPIOS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_DATA_MUNICIPIOS}
  echo "commit;" >> ${FILE_NAME_DATA_MUNICIPIOS}

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --section post-data --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_POST_MUNICIPIOS}" --table "${TABLE_NAME_MUNICIPIOS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_POST_MUNICIPIOS}
  echo "commit;" >> ${FILE_NAME_POST_MUNICIPIOS}
)

# Focos
(
  echo "# Dumping Focos..."
  echo ""

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --clean --section pre-data --schema-only --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_SCHEMA_FOCOS}" --table "${TABLE_NAME_FOCOS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_SCHEMA_FOCOS}
  echo "commit;" >> ${FILE_NAME_SCHEMA_FOCOS}

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --section data --column-inserts --data-only --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_DATA_FOCOS_TMP}" --table "${TABLE_NAME_FOCOS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_DATA_FOCOS_TMP}
  echo "commit;" >> ${FILE_NAME_DATA_FOCOS_TMP}
  sed '0~500000 s/$/\ncommit;\nbegin;/g' < ${FILE_NAME_DATA_FOCOS_TMP} > ${FILE_NAME_DATA_FOCOS}

  rm ${FILE_NAME_DATA_FOCOS_TMP}

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --section post-data --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_POST_FOCOS}" --table "${TABLE_NAME_FOCOS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_POST_FOCOS}
  echo "commit;" >> ${FILE_NAME_POST_FOCOS}

  /usr/bin/pg_dump --host ${HOST} --port ${PORT} --username "${USERNAME}" --format plain --no-owner --create --clean --section pre-data --section data --section post-data --no-privileges --no-tablespaces --no-unlogged-table-data --file "${FILE_NAME_VIEW_FOCOS}" --table "${VIEW_NAME_FOCOS}" "${DATABASE}"

  sed -i '1s/^/begin transaction;\n\n/' ${FILE_NAME_VIEW_FOCOS}
  echo "commit;" >> ${FILE_NAME_VIEW_FOCOS}
)

echo "# Compressing files..."
echo ""

tar -czf ${TAR_GZ_FILENAME} *.sql

echo "# Finishing..."
echo ""

mv ${TAR_GZ_FILENAME} ../.

cd ..

rm -r ${TMP_FOLDER}

echo "####################################"
echo "# Dump finished!"
echo "####################################"
echo ""
echo $(date)
echo ""

exit 0
