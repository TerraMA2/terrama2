module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('alert_additional_data', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      referrer_attribute: Sequelize.STRING,
      referred_attribute: Sequelize.STRING,
      data_attributes: Sequelize.TEXT,
      alert_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'alerts',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'CASCADE'
      },
      dataseries_id: {
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
      },
      data_set_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'data_sets',
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
    return queryInterface.dropTable({ tableName: 'alert_additional_data', schema: 'terrama2' });
  }
};