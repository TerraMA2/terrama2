module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('data_series_semantics', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      code: {
        type: Sequelize.STRING,
        allowNull: false,
        unique: true
      },
      data_format_name: {
        type: Sequelize.STRING,
        references: {
          key: 'name',
          model: {
            tableName: 'data_formats',
            schema: 'terrama2'
          }
        },
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
      data_series_type_name: {
        type: Sequelize.STRING,
        references: {
          key: 'name',
          model: {
            tableName: 'data_series_types',
            schema: 'terrama2'
          }
        },
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'data_series_semantics', schema: 'terrama2' });
  }
};