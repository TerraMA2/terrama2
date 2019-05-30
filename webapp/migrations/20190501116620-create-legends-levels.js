module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('legend_levels', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: Sequelize.STRING,
      value: Sequelize.DOUBLE,
      level: Sequelize.INTEGER.UNSIGNED,
      legend_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'legends',
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
    return queryInterface.dropTable({ tableName: 'legend_levels', schema: 'terrama2' });
  }
};