module.exports = function(sequelize, DataTypes) {
  var Version = sequelize.define("Version",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      major: {
        type: DataTypes.INTEGER,
        allowNull: false
      },
      minor: {
        type: DataTypes.INTEGER,
        allowNull: false
      },
      patch: {
        type: DataTypes.INTEGER,
        allowNull: false
      },
      tag: {
        type: DataTypes.TEXT,
        allowNull: false
      },
      database: {
        type: DataTypes.INTEGER,
        allowNull: false
      },
      insert_time: {
        type: DataTypes.DATE,
        allowNull: false,
        defaultValue: sequelize.literal('CURRENT_TIMESTAMP')
      }
    },
    {
      tableName: 'version',
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      indexes: [
        {
          name: "full_version",
          unique: true,
          fields: ['major', 'minor', 'patch']
        },
        {
          name: "full_database_version",
          unique: true,
          fields: ['major', 'minor', 'database']
        }
      ]
    }
  );

  return Version;
};
