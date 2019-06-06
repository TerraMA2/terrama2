module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('projects', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      version: Sequelize.INTEGER,
      name: {
        type: Sequelize.STRING,
        unique: true,
        allowNull: false,
        comment: "Project name"
      },
      description: {
        type: Sequelize.TEXT,
        comment: "Project description."
      },
      protected: Sequelize.BOOLEAN,
      active: Sequelize.BOOLEAN,
      user_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'users',
            schema: "terrama2"
          },
        },
        allowNull: false,
        onDelete: 'CASCADE'
      }
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'projects', schema: 'terrama2' });
  }
};