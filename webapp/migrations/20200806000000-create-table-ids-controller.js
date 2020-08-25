module.exports = {
    up: function(queryInterface, Sequelize) {
      return queryInterface.createTable('table_ids_controller', {
        id: {
          type: Sequelize.INTEGER,
          allowNull: false,
          primaryKey: true,
          autoIncrement: true
        },
        table_name: Sequelize.STRING,
        last_id: Sequelize.INTEGER
      }, { schema: 'terrama2' });
    },
    down: function(queryInterface, /*Sequelize*/) {
      return queryInterface.dropTable({ tableName: 'table_ids_controller', schema: 'terrama2' });
    }
  };