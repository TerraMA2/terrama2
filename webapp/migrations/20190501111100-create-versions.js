module.exports = {
  up: function(queryInterface, Sequelize) {
    return queryInterface.createTable('version', {
      id: {
        type: Sequelize.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      major: {
        type: Sequelize.INTEGER,
        allowNull: false
      },
      minor: {
        type: Sequelize.INTEGER,
        allowNull: false
      },
      patch: {
        type: Sequelize.INTEGER,
        allowNull: false
      },
      tag: {
        type: Sequelize.TEXT,
        allowNull: false
      },
      database: {
        type: Sequelize.INTEGER,
        allowNull: false
      },
      insert_time: {
        type: Sequelize.DATE,
        allowNull: false,
        defaultValue: Sequelize.literal('CURRENT_TIMESTAMP')
      }
    }, { schema: 'terrama2' })
    .then(() => (
      queryInterface.addIndex(
        { schema: 'terrama2', tableName: 'version' },
        ['major', 'minor', 'patch'],
        { type: 'unique' }
      )
    ))
    .then(() => (
      queryInterface.addIndex(
        { schema: 'terrama2', tableName: 'version' },
        ['major', 'minor', 'database'],
        { type: 'unique' }
      )
    ));
  },
  down: function(queryInterface, /*Sequelize*/) {
    return queryInterface.dropTable({ tableName: 'version', schema: 'terrama2' });
  }
};