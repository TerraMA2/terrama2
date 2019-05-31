'use strict';

const { DataProviderIntentId } = require('./../core/Enums')

const intents = [
  { id: DataProviderIntentId.COLLECT, name: "COLLECT" },
  { id: DataProviderIntentId.PROCESSING, name: "PROCESSING" }
];

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'data_provider_intents'}, intents);
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
      { schema: 'terrama2', tableName: 'data_provider_intents'},
      null, {});
  }
};
