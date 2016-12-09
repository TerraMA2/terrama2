module.exports = function(sequelize, DataTypes) {

  var DataSeriesType = require('../core/Enums').DataSeriesType;
  var DataSet = sequelize.define("DataSet",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      active: DataTypes.BOOLEAN
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      instanceMethods: {
        getDataSet: function(dataSetType) {
          if (typeof(dataSetType) === 'string') {
            switch (dataSetType) {
              case DataSeriesType.DCP:
                return this.getDataSetDcp();
              case DataSeriesType.OCCURRENCE:
                return this.getDataSetOccurrence();
              case DataSeriesType.GRID:
                return this.getDataSetGrid();
              case DataSeriesType.ANALYSIS_MONITORED_OBJECT:
                return this.getDataSetMonitored();
              default:
                return null;
            }
          } else
            throw "Invalid data set type";
        }
      },

      classMethods: {
        associate: function(models) {
          DataSet.belongsTo(models.DataSeries, {
            foreignKey: {
              name: "data_series_id",
              onDelete: "CASCADE",
              foreignKey: {
                allowNull: false
              }
            }
          });

          DataSet.hasOne(models.DataSetDcp, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataSet.hasOne(models.DataSetOccurrence, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataSet.hasOne(models.DataSetGrid, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataSet.hasOne(models.DataSetMonitored, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataSet.hasOne(models.Analysis, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'dataset_output',
              allowNull: false
            }
          });

          DataSet.hasMany(models.DataSetFormat, {
            onDelete: 'CASCADE',
            foreignKey: {
              allowNull: false
            }
          })
        }
      }
    }
  );

  return DataSet;
};
