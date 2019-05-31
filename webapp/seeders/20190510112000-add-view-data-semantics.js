'use strict';

const Application = require('../core/Application');

module.exports = {
  up: async function (queryInterface, /*Sequelize*/) {
    const filterSemantic = Application.get('semantics').find(semantic => semantic.code === 'STATIC_DATA-VIEW-postgis');
    const semantic = [{
      code: filterSemantic.code,
      data_format_name: filterSemantic.format,
      data_series_type_name: filterSemantic.type
    }]

    // Insert Semantics
    await queryInterface.bulkInsert({ schema: 'terrama2', tableName: 'data_series_semantics' }, semantic);
    const savedSemantic = await queryInterface.sequelize.query(
      `SELECT id, code FROM terrama2.data_series_semantics WHERE code = '${filterSemantic.code}'`
    )

    const types = filterSemantic.providers_type_list.map(elm => `'${elm}'`).join();

    // Insert Semantic Providers
    const providerTypes = await queryInterface.sequelize.query(
      `SELECT id, name FROM terrama2.data_provider_types WHERE name IN ( ${types} ) `
    )

    const semanticsProviderTypes = []

    for(let providerType of filterSemantic.providers_type_list) {
      const matchedType = providerTypes[0].find(internalType => internalType.name === providerType);

      if (!matchedType)
        continue;

      semanticsProviderTypes.push({
        data_provider_type_id: matchedType.id,
        data_series_semantics_id: savedSemantic[0][0].id
      })
    }

    return queryInterface.bulkInsert({
      schema: 'terrama2',
      tableName: 'semantics_providers_type'
    }, semanticsProviderTypes);
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
