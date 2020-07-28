'use strict';

const Application = require('../core/Application');

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    // Prepare context to insert multiple semantics
    const bulkData = [
      { name: 'CSV', description: 'test'},
      { name: 'OCCURRENCE', description: 'test'},
      { name: 'GRID', description: 'test'},
      { name: 'POSTGIS', description: 'test'},
      { name: 'OGR', description: 'test'},
      { name: 'GDAL', description: 'test'},
      { name: 'GRADS', description: 'test'},
    ]

    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'data_formats'}, bulkData);
  },

  down: function (queryInterface, /*Sequelize*/) {
    /*
      Add reverting commands here.
      Return a promise to correctly handle asynchronicity.

      Example:
      return queryInterface.bulkDelete('Person', null, {});
    */
    return queryInterface.bulkDelete({ schema: 'terrama2', tableName: 'data_formats'}, null, {});
  }
};
