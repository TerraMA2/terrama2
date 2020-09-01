'use strict';

const { ServiceType } = require('../core/Enums');
const Application = require('./../core/Application')

module.exports = {
  up: async function (queryInterface, /*Sequelize*/) {
    const settings = Application.getContextConfig();
    const { database, host, password, username, port } = settings.db;
    const interpolator = {
      name: "Local Interpolator",
      description: "Local service for Interpolator",
      port: 6547,
      pathToBinary: "terrama2_service",
      numberOfThreads: 0,
      service_type_id: ServiceType.INTERPOLATION
    }

    if (settings.services) {
      interpolator.host = settings.services.interpolator.host;
      interpolator.sshPort = settings.services.interpolator.sshPort;
      interpolator.sshUser = settings.services.interpolator.sshUser;
    }

    await queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'service_instances'}, [interpolator]);

    const insertedService = await queryInterface.sequelize.query(
      `SELECT id FROM terrama2.service_instances WHERE name = '${interpolator.name}'`
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
      { name: interpolator.name, service_type_id: ServiceType.INTERPOLATION }, {});
  }
};
