module.exports = function(sequelize, DataTypes) {
  var LogStatus = sequelize.define("LogStatus",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      status: DataTypes.STRING
    },
    {
      freezeTableName: true,
      tableName: 'log_status',
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          LogStatus.hasMany(models.DataCollectionLog, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'status_id',
              allowNull: false
            }
          });
        }
      }
    }
  );

  return LogStatus;
};