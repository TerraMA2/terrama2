'use strict';

const { AnalysisDataSeriesType } = require('./../core/Enums')

const analysisDataSeriesTypes = [
  { id: AnalysisDataSeriesType.DATASERIES_DCP_TYPE, name: "Dcp" },
  { id: AnalysisDataSeriesType.DATASERIES_GRID_TYPE, name: "Grid" },
  { id: AnalysisDataSeriesType.DATASERIES_MONITORED_OBJECT_TYPE, name: "Monitored Object" },
  { id: AnalysisDataSeriesType.ADDITIONAL_DATA_TYPE, name: "Additional Data" }
];

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'analysis_data_series_types'}, analysisDataSeriesTypes);
  },

  down: function (queryInterface, Sequelize) {
    /*
      Add reverting commands here.
      Return a promise to correctly handle asynchronicity.

      Example:
      return queryInterface.bulkDelete('Person', null, {});
    */
    // TODO: Remove null and set where condition to remove only the
    // inserted records by this seed
    return queryInterface.bulkDelete(
      { schema: 'terrama2', tableName: 'analysis_data_series_types'},
      null, {});
  }
};
