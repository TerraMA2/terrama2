'use strict';

const { InterpolationMethod } = require('./../core/Enums')

const interpolations = [
  { id: InterpolationMethod.NEAREST_NEIGHBOR.value, name: InterpolationMethod.NEAREST_NEIGHBOR.name },
  { id: InterpolationMethod.BI_LINEAR.value, name: InterpolationMethod.BI_LINEAR.name },
  { id: InterpolationMethod.BI_CUBIC.value, name: InterpolationMethod.BI_CUBIC.name }
];

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'interpolation_method'}, interpolations);
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
      { schema: 'terrama2', tableName: 'interpolation_method'},
      null, {});
  }
};
