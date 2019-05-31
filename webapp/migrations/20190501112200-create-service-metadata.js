module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('service_metadata', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      key: Sequelize.STRING,
      value: Sequelize.TEXT,
      service_instance_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'service_instances',
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
    return queryInterface.dropTable({ tableName: 'service_metadata', schema: 'terrama2' });
  }
};