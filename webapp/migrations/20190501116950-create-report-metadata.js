module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('report_metadata', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      title: Sequelize.STRING,
      abstract: Sequelize.STRING,
      description: Sequelize.TEXT,
      author: Sequelize.STRING,
      contact: Sequelize.STRING,
      copyright: Sequelize.STRING,
      timestamp_format: Sequelize.STRING,
      logo_path: Sequelize.STRING,
      document_format: Sequelize.STRING,
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
    return queryInterface.dropTable({ tableName: 'report_metadata', schema: 'terrama2' });
  }
};