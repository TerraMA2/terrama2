module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('service_types', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: Sequelize.STRING,
        unique: true
      },
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'service_types', schema: 'terrama2' });
  }
};