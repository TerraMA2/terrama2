module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('view_style_legend_metadata', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true,
        comment: "ViewStyleLegendMetadata identifier"
      },
      key: Sequelize.STRING,
      value: Sequelize.TEXT,
      legend_id: {
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
    return queryInterface.dropTable({ tableName: 'view_style_legend_metadata', schema: 'terrama2' });
  }
};