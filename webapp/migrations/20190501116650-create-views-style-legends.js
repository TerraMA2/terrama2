module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('view_style_legends', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true,
        comment: "ViewStyleLegend identifier"
      },
      type: {
        type: Sequelize.INTEGER,
        allowNull: false,
        comment: "Target type to generate legend (Gradient, Interval and Value)"
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
    return queryInterface.dropTable({ tableName: 'view_style_legends', schema: 'terrama2' });
  }
};