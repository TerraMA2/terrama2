'use strict';

const { InterpolatorStrategy } = require('./../core/Enums')

const languages = [
  { id: InterpolatorStrategy.NEAREST_NEIGHBOR.code, name: InterpolatorStrategy.NEAREST_NEIGHBOR.code },
  { id: InterpolatorStrategy.AVERAGE_NEIGHBOR.code, name: InterpolatorStrategy.AVERAGE_NEIGHBOR.code},
  { id: InterpolatorStrategy.W_AVERAGE_NEIGHBOR.code, name: InterpolatorStrategy.W_AVERAGE_NEIGHBOR.code }
];

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'interpolator_strategy'}, languages);
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
      { schema: 'terrama2', tableName: 'interpolator_strategy'},
      null, {});
  }
};
