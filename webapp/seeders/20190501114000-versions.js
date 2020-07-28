'use strict';

const Application = require('./../core/Application');
const versionRecord = Application.getVersion(false);

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'version'}, [versionRecord]);
  },

  down: function (queryInterface, Sequelize) {
    /*
      Add reverting commands here.
      Return a promise to correctly handle asynchronicity.

      Example:
      return queryInterface.bulkDelete('Person', null, {});
    */
    return queryInterface.bulkDelete(
      { schema: 'terrama2', tableName: 'version'},
      { ...versionRecord }, {});
  }
};
