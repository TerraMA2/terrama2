'use strict';

const { DataProviderType } = require('./../core/Enums')

const intents = [
  { id: DataProviderType.HTTPS.value, name: DataProviderType.HTTPS.name }
];

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'data_provider_types'}, intents);
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
      { schema: 'terrama2', tableName: 'data_provider_types'},
      null, {});
  }
};