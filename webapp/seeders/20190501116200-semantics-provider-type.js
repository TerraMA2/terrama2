'use strict';

const Application = require('./../core/Application')

module.exports = {
  up: async function (queryInterface, /*Sequelize*/) {
    const semanticsInMemory = Application.get("semantics");

    const semanticsInDatabase = await queryInterface.sequelize.query(
      `SELECT id, code FROM terrama2.data_series_semantics`
    );

    const providerTypes = await queryInterface.sequelize.query(
      `SELECT id, name FROM terrama2.data_provider_types`
    )

    const semanticsProviderTypes = []

    for(let semantic of semanticsInMemory) {
      const matchedSemantic = semanticsInDatabase[0].find(
        internalSemantic => internalSemantic.code === semantic.code);

      if (!matchedSemantic) {
        console.log(`The semantic ${semantic.code} does not exists in database...`);
        continue;
      }

      const semanticId = matchedSemantic.id;

      for(let providerType of semantic.providers_type_list) {
        const matchedType = providerTypes[0].find(internalType => internalType.name === providerType);

        if (!matchedType)
          continue;

        semanticsProviderTypes.push({
          data_provider_type_id: matchedType.id,
          data_series_semantics_id: semanticId
        })
      }
    }

    return queryInterface.bulkInsert({
      schema: 'terrama2',
      tableName: 'semantics_providers_type'
    }, semanticsProviderTypes);
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
      { schema: 'terrama2', tableName: 'semantics_providers_type'},
      null, {});
  }
};
