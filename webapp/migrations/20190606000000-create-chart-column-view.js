module.exports = {
  up: function(queryInterface, Sequelize) {
      return queryInterface.addColumn(
        { schema: 'terrama2', tableName: 'views' },
        'charts',
        {
          type: Sequelize.JSONB,
          allowNull: true,
        }
      )
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.removeColumn({
      tableName: 'views',
      schema: 'terrama2'
    }, 'charts');
  }
  
};