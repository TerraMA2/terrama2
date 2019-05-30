module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('alert_notifications', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      include_report: Sequelize.STRING,
      notify_on_change: Sequelize.BOOLEAN,
      simplified_report: Sequelize.BOOLEAN,
      notify_on_legend_level: Sequelize.INTEGER,
      recipients: Sequelize.TEXT,
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
    return queryInterface.dropTable({ tableName: 'alert_notifications', schema: 'terrama2' });
  }
};