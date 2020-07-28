'use strict';

const { ServiceType } = require('../core/Enums');
const Application = require('./../core/Application')

const analysis = {
  name: "Local Analysis",
  description: "Local service for Analysis",
  port: 6544,
  pathToBinary: "terrama2_service",
  numberOfThreads: 0,
  service_type_id: ServiceType.ANALYSIS
}

module.exports = {
  up: async function (queryInterface, /*Sequelize*/) {
    const { db } = Application.getContextConfig();
    const { database, host, password, username, port } = db;

    await queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'service_instances'}, [analysis]);

    const insertedService = await queryInterface.sequelize.query(
      `SELECT id FROM terrama2.service_instances WHERE name = '${analysis.name}'`
    )

    const log = {
      host,
      port: port || 5432,
      user: username,
      password,
      database,
      service_instance_id: insertedService[0][0].id
    }

    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'logs' }, [log])
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
      { schema: 'terrama2', tableName: 'service_instances'},
      { name: analysis.name, service_type_id: ServiceType.ANALYSIS }, {});
  }
};
