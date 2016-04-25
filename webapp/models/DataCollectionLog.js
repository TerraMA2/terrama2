module.exports = function(sequelize, DataTypes) {
  var DataCollectionLog = sequelize.define("DataCollectionLog",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      origin_uri: DataTypes.STRING,
      storage_uri: DataTypes.STRING,
      data_timestamp: DataTypes.TIME,
      collect_timestamp: DataTypes.TIME
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          DataCollectionLog.belongsTo(models.LogStatus, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'status_id',
              allowNull: false
            }
          });

          DataCollectionLog.belongsTo(models.CollectorInputOutput, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataCollectionLog;
};