'use strict';

const { ScriptLanguage } = require('./../core/Enums')

const languages = [
  { id: ScriptLanguage.PYTHON, name: "PYTHON" },
  { id: ScriptLanguage.LUA, name: "LUA" }
];

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'script_languages'}, languages);
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
      { schema: 'terrama2', tableName: 'script_languages'},
      null, {});
  }
};
