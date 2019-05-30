module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('interpolator_metadata', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true,
        comment: "InterpolatorMetadata identifier"
      },
      key: Sequelize.STRING,
      value: Sequelize.TEXT,
      interpolator_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'interpolators',
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
    return queryInterface.dropTable({ tableName: 'interpolator_metadata', schema: 'terrama2' });
  }
};