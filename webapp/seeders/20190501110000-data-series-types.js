'use strict';

const Application = require('../core/Application');

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    // Prepare context to insert multiple semantics
    const bulkData = [
      { name: 'DCP', description: 'test'},
      { name: 'OCCURRENCE', description: 'test'},
      { name: 'GRID', description: 'test'},
      { name: 'ANALYSIS_MONITORED_OBJECT', description: 'test'},
      { name: 'GEOMETRIC_OBJECT', description: 'test'},
      { name: 'VECTOR_PROCESSING_OBJECT', description: 'test'},
    ]

    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'data_series_types'}, bulkData);
  },

  down: function (queryInterface, /*Sequelize*/) {
    /*
      Add reverting commands here.
      Return a promise to correctly handle asynchronicity.

      Example:
      return queryInterface.bulkDelete('Person', null, {});
    */
    return queryInterface.bulkDelete({ schema: 'terrama2', tableName: 'data_series_types'}, null, {});
  }
};
