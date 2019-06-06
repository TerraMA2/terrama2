module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('filters', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      frequency: Sequelize.INTEGER,
      frequency_unit: Sequelize.STRING,
      discard_before: Sequelize.DATE,
      discard_after: Sequelize.DATE,
      region: Sequelize.GEOMETRY('POLYGON'),
      by_value: Sequelize.DECIMAL,
      crop_raster: Sequelize.BOOLEAN,
      data_series_id: Sequelize.INTEGER,
      collector_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'collectors',
            schema: "terrama2"
          },
        },
        allowNull: false,
        unique: true,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
      data_series_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'data_series',
            schema: "terrama2"
          },
        },
        allowNull: true,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      }
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'filters', schema: 'terrama2' });
  }
};