'use strict';

const { ResolutionType } = require('./../core/Enums')

const resolutionTypes = [
  { id: ResolutionType.BIGGEST_GRID.value, name: ResolutionType.BIGGEST_GRID.name },
  { id: ResolutionType.SMALLEST_GRID.value, name: ResolutionType.SMALLEST_GRID.name },
  { id: ResolutionType.SAME_FROM_DATA_SERIES.value, name: ResolutionType.SAME_FROM_DATA_SERIES.name },
  { id: ResolutionType.CUSTOM.value, name: ResolutionType.CUSTOM.name },
];

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'analysis_resolution_types'}, resolutionTypes);
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
      { schema: 'terrama2', tableName: 'analysis_resolution_types'},
      null, {});
  }
};
