'use strict';

const { ServiceType } = require('../core/Enums');
const Application = require('./../core/Application')

const view = {
  name: "Local View",
  description: "Local service for view",
  port: 6545,
  pathToBinary: "terrama2_service",
  numberOfThreads: 0,
  service_type_id: ServiceType.VIEW
}

module.exports = {
  up: async function (queryInterface, /*Sequelize*/) {
    const { db } = Application.getContextConfig();
    const { database, host, password, username, port } = db;

    await queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'service_instances'}, [view]);

    const insertedService = await queryInterface.sequelize.query(
      `SELECT id FROM terrama2.service_instances WHERE name = '${view.name}'`
    )

    const id = insertedService[0][0].id;

    const log = {
      host,
      port: port || 5432,
      user: username,
      password,
      database,
      service_instance_id: id
    }

    await queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'logs' }, [log]);

    const metadata = {
      key: 'maps_server',
      service_instance_id: id,
      value: 'http://admin:geoserver@localhost:8080/geoserver'
    }

    return queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'service_metadata' }, [metadata]);
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
      { name: view.name, service_type_id: ServiceType.VIEW }, {});
  }
};
