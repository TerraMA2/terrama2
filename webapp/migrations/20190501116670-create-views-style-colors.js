module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('view_style_colors', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true,
        comment: "ViewStyleColor identifier"
      },
      title: {
        type: Sequelize.STRING,
        allowNull: false,
        comment: "Title"
      },
      color: {
        type: Sequelize.STRING,
        comment: "Style Color Hex"
      },
      value: {
        type: Sequelize.STRING,
        allowNull: true,
        comment: "Style value"
      },
      isDefault: {
        type: Sequelize.BOOLEAN,
        allowNull: false,
        default: false
      },
      view_style_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'view_style_legends',
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
    return queryInterface.dropTable({ tableName: 'view_style_colors', schema: 'terrama2' });
  }
};