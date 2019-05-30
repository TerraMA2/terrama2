module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('registered_views', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      workspace: {
        type: Sequelize.STRING,
        allowNull: false,
        comment: "Map server workspace"
      },
      uri: {
        type: Sequelize.STRING,
        allowNull: false,
        comment: "Map server URI"
      },
      created_at: {
        allowNull: false,
        type: Sequelize.DATE
      },
      updated_at: {
        allowNull: false,
        type: Sequelize.DATE
      },
      view_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'views',
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
    return queryInterface.dropTable({ tableName: 'registered_views', schema: 'terrama2' });
  }
};