module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('alert_attachments', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      y_max: {
        type: Sequelize.DOUBLE,
        allowNull: false
      },
      y_min: {
        type: Sequelize.DOUBLE,
        allowNull: false
      },
      x_max: {
        type: Sequelize.DOUBLE,
        allowNull: false
      },
      x_min: {
        type: Sequelize.DOUBLE,
        allowNull: false
      },
      srid: {
        type: Sequelize.INTEGER,
        allowNull: false
      },
      width: {
        type: Sequelize.INTEGER,
        allowNull: true
      },
      height: {
        type: Sequelize.INTEGER,
        allowNull: true
      },
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
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      }
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'alert_attachments', schema: 'terrama2' });
  }
};