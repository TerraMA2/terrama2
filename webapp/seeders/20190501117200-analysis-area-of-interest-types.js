'use strict';

const { InterestAreaType } = require('./../core/Enums')

const analysisDataSeriesTypes = [
  { id: InterestAreaType.UNION.value, name: InterestAreaType.UNION.name },
  { id: InterestAreaType.SAME_FROM_DATA_SERIES.value, name: InterestAreaType.SAME_FROM_DATA_SERIES.name },
  { id: InterestAreaType.CUSTOM.value, name: InterestAreaType.CUSTOM.name },
];

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'analysis_area_of_interest_types'}, analysisDataSeriesTypes);
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
      { schema: 'terrama2', tableName: 'analysis_area_of_interest_types'},
      null, {});
  }
};
