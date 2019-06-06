module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('data_provider_intents', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: Sequelize.STRING,
        unique: true,
      },
      description: Sequelize.TEXT
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'data_provider_intents', schema: 'terrama2' });
  }
};