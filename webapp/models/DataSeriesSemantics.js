module.exports = function(sequelize, DataTypes) {
  var DataSeriesSemantics = sequelize.define("DataSeriesSemantics",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      code: {
        type: DataTypes.STRING,
        allowNull: false,
        unique: true
      }
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          DataSeriesSemantics.belongsTo(models.DataSeriesType, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataSeriesSemantics.belongsTo(models.DataFormat, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          DataSeriesSemantics.hasMany(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_semantics_id",
              allowNull: false
            }
          });

          DataSeriesSemantics.hasMany(models.SemanticsProvidersType, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'data_series_semantics_id',
              allowNull: false
            }
          });

          DataSeriesSemantics.hasMany(models.SemanticsMetadata, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'data_series_semantics_id',
              allowNull: false
            }
          });
        }
      }
    }
  );

  return DataSeriesSemantics;
};
