module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('data_providers', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      uri: Sequelize.STRING,
      name: {
        type: Sequelize.STRING
      },
      description: Sequelize.TEXT,
      active: {
        type: Sequelize.BOOLEAN,
        allowNull: false
      },
      project_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'projects',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
      data_provider_type_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'data_provider_types',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
      data_provider_intent_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'data_provider_intents',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      }
    },
    {
      schema: 'terrama2'
    })
    .then(() => queryInterface.addIndex({ schema: 'terrama2', tableName: 'data_providers' }, ['project_id', 'name'], { type: 'unique' }));
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'data_providers', schema: 'terrama2' });
  }
};