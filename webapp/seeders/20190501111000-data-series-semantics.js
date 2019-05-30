'use strict';

const Application = require('./../core/Application');

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    // Retrieve all semantics
    const semantics = Application.get("semantics");
    // Prepare context to insert multiple semantics
    const bulkSemantics = semantics.map(semantic => (
      {
        code: semantic.code,
        data_format_name: semantic.format,
        data_series_type_name: semantic.type
      })
    )

    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'data_series_semantics'}, bulkSemantics);
  },

  down: function (queryInterface, /*Sequelize*/) {
    /*
      Add reverting commands here.
      Return a promise to correctly handle asynchronicity.

      Example:
      return queryInterface.bulkDelete('Person', null, {});
    */
    return queryInterface.bulkDelete({ schema: 'terrama2', tableName: 'data_series_semantics'}, null, {});
  }
};
