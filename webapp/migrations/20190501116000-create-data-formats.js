module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('data_formats', {
      name: {
        type: Sequelize.STRING,
        allowNull: false,
        primaryKey: true
      },
      description: Sequelize.STRING
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'data_formats', schema: 'terrama2' });
  }
};