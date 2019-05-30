module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('service_instances', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: Sequelize.STRING,
        unique: true
      },
      host: Sequelize.STRING,
      port: Sequelize.INTEGER,
      sshUser: Sequelize.STRING,
      sshPort: {
        type: Sequelize.INTEGER,
        default: 22
      },
      description: Sequelize.TEXT,
      pathToBinary: Sequelize.STRING,
      numberOfThreads: Sequelize.INTEGER,
      runEnviroment: Sequelize.STRING,
      service_type_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'service_types',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
      created_at: {
        allowNull: false,
        type: Sequelize.DATE,
        defaultValue: Sequelize.literal('CURRENT_TIMESTAMP')
      },
      updated_at: {
        allowNull: false,
        type: Sequelize.DATE,
        defaultValue: Sequelize.literal('CURRENT_TIMESTAMP')
      }
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'service_instances', schema: 'terrama2' });
  }
};