'use strict';

const Application = require('./../core/Application');
const logger = require('./../core/Logger');

const semanticsSupported = [
  /*'DCP-json_cemaden'*/, 'DCP-generic', 'DCP-inpe',
  'DCP-postgis','DCP-single_table', 'DCP-toa5',
  'GEOMETRIC_OBJECT-ogr', 'GEOMETRIC_OBJECT-wfe',
  'STATIC_DATA-ogr', 'GEOMETRIC_OBJECT-postgis',
  'STATIC_DATA-postgis',
  'GRID-gdal', 'GRID-static_gdal', 'GRID-geotiff', 'GRID-ascii',
  'NETCDF-nc', 'GRID-grads', 'GRID-grib',
  'ANALYSIS_MONITORED_OBJECT-postgis',
  'Occurrence-generic', 'OCCURRENCE-wfp',
  /*'OCCURRENCE-lightning'*/, 'OCCURRENCE-postgis'
]

module.exports = {
  up: async function (queryInterface, /*Sequelize*/) {
    // Retrieve all semantics
    const semantics = Application.get("semantics");

    const dbSemantics = await queryInterface.sequelize.query(
      `SELECT code FROM terrama2.data_series_semantics`
    )

    // Prepare context to insert multiple semantics
    let bulkSemantics = []

    for(let semantic of semantics) {
      // Skip inserted semantic
      if (dbSemantics[0].find(internalSemantic => internalSemantic.code === semantic.code)) {
        logger.debug(`Skip ${semantic.code}.`)
        continue;
      }

      // only insert semantics supported in this seed version
      if (semanticsSupported.includes(semantic.code)) {
        bulkSemantics.push({
          code: semantic.code,
          data_format_name: semantic.format,
          data_series_type_name: semantic.type
        })
      } else {
        logger.info(`Not supported ${semantic.code}`)
      }
    }

    if (bulkSemantics.length > 0)
      return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'data_series_semantics'}, bulkSemantics);
    return Promise.resolve();
  },

  down: function (queryInterface, /*Sequelize*/) {
    /*
      Add reverting commands here.
      Return a promise to correctly handle asynchronicity.

      Example:
      return queryInterface.bulkDelete('Person', null, {});
    */
    return queryInterface.bulkDelete({ schema: 'terrama2', tableName: 'data_series_semantics'}, null, {});
  }
};
