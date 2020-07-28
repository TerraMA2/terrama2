module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('semantics_providers_type', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      data_provider_type_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'data_provider_types',
            schema: 'terrama2'
          },
          allowNull: false
        },
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
      data_series_semantics_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'data_series_semantics',
            schema: 'terrama2'
          }
        },
        onDelete: 'CASCADE',
        onUpdate: 'CASCADE'
      },
    }, { schema: 'terrama2' });
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'semantics_providers_type', schema: 'terrama2' });
  }
};