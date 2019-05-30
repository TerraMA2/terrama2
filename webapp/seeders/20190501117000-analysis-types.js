'use strict';

const { AnalysisType } = require('./../core/Enums')

const analysisTypes = [
  { id: AnalysisType.DCP, name: "Dcp" },
  { id: AnalysisType.GRID, name: "Grid" },
  { id: AnalysisType.MONITORED, name: "Monitored Object" },
  { id: AnalysisType.VP, name: "Vector Processing" }
];

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'analysis_types'}, analysisTypes);
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
      { schema: 'terrama2', tableName: 'analysis_types'},
      null, {});
  }
};
