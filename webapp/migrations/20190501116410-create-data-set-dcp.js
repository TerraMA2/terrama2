module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('data_set_dcps', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      position: Sequelize.GEOMETRY('Point'),
      data_set_id: {
        type: Sequelize.INTEGER,
        references: {
          key: 'id',
          model: {
            tableName: 'data_sets',
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
    return queryInterface.dropTable({ tableName: 'data_set_dcps', schema: 'terrama2' });
  }
};