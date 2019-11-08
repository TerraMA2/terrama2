module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.changeColumn(
      { schema: 'terrama2', tableName: 'analysis_metadata' },
      'value',
      {
        type: Sequelize.TEXT
      },);
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.changeColumn(
      { schema: 'terrama2', tableName: 'analysis_metadata' },
      'value',
      {
        type: Sequelize.TEXT
      });
  }
};