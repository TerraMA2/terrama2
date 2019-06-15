module.exports = {
  up: function(queryInterface, Sequelize) {
      return queryInterface.changeColumn(
        { schema: 'terrama2', tableName: 'views' },
        'charts',
        {
          type: Sequelize.JSONB,
          allowNull: false,
          defaultValue: "[]"
        }
      )
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.changeColumn({
      tableName: 'views',
      schema: 'terrama2'
    }, 'charts',
    {
      type: Sequelize.JSONB,
      allowNull: true
    }
    );
  }
  
};