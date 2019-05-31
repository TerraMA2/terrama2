'use strict';

const { DataProviderType } = require('./../core/Enums')

const intents = [
  { id: DataProviderType.FILE.value, name: DataProviderType.FILE.name },
  { id: DataProviderType.FTP.value, name: DataProviderType.FTP.name },
  { id: DataProviderType.HTTP.value, name: DataProviderType.HTTP.name },
  { id: DataProviderType.POSTGIS.value, name: DataProviderType.POSTGIS.name },
  { id: DataProviderType.STATIC_HTTP.value, name: DataProviderType.STATIC_HTTP.name }
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
