module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('data_provider_options', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      key: Sequelize.STRING,
      value: Sequelize.TEXT,
      data_provider_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'data_providers',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'CASCADE'
      }
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'data_provider_options', schema: 'terrama2' });
  }
};