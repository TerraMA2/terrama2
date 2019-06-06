'use strict';

module.exports = {
  up: function (queryInterface, /*Sequelize*/) {
    const bcrypt = require('bcrypt');
    const salt = bcrypt.genSaltSync(10);
    const password = bcrypt.hashSync('admin', salt);
    const token = bcrypt.hashSync('admin@terrama2.inpe.bradmin', 10)
    // Retrieve all semantics
    const adminUser = {
      name: "Administrator",
      username: "admin",
      password,
      salt,
      cellphone: '14578942362',
      email: 'admin@terrama2.inpe.br',
      administrator: true,
      token
    }
    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'users'}, [adminUser]);
  },

  down: function (queryInterface, /*Sequelize*/) {
    /*
      Add reverting commands here.
      Return a promise to correctly handle asynchronicity.

      Example:
      return queryInterface.bulkDelete('Person', null, {});
    */
    return queryInterface.bulkDelete({ schema: 'terrama2', tableName: 'users'}, { username: 'admin' }, {});
  }
};
