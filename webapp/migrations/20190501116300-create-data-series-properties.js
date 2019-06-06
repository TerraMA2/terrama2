module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('data_series_properties', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      attribute: Sequelize.STRING,
      alias: Sequelize.STRING,
      position: Sequelize.INTEGER,
      data_series_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'data_series',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      }
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'data_series_properties', schema: 'terrama2' });
  }
};