module.exports = function(sequelize, DataTypes)
{
  var ReprocessingHistoricalData = sequelize.define("ReprocessingHistoricalData", {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      startDate: {
        type: DataTypes.DATE,
        allowNull: true
      },
      endDate: {
        type: DataTypes.DATE,
        allowNull: true
      }
    },
    {
      freezeTableName: true,
      tableName: 'reprocessing_historical_data',
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          ReprocessingHistoricalData.belongsTo(models.Schedule, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "schedule_id",
              allowNull: false
            }
          });
        }
      }
    }
  );

  return ReprocessingHistoricalData;
};