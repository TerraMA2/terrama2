module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('analysis_metadata', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      key: Sequelize.STRING,
      value: Sequelize.STRING,
      analysis_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'analysis',
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
    return queryInterface.dropTable({ tableName: 'analysis_metadata', schema: 'terrama2' });
  }
};