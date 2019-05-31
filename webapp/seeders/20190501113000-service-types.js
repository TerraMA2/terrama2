'use strict';

const { ServiceType } = require('./../core/Enums')

const types = [
  { id: ServiceType.COLLECTOR, name: "COLLECT" },
  { id: ServiceType.ANALYSIS, name: "ANALYSIS" },
  { id: ServiceType.VIEW, name: "VIEW" },
  { id: ServiceType.ALERT, name: "ALERT" },
  { id: ServiceType.INTERPOLATION, name: "INTERPOLATION" }
];

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'service_types'}, types);
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
      { schema: 'terrama2', tableName: 'service_types'},
      null, {});
  }
};
