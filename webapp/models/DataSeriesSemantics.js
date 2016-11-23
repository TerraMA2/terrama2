module.exports = function(sequelize, DataTypes) {
  var DataSeriesSemantics = sequelize.define("DataSeriesSemantics",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: DataTypes.STRING,
        allowNull: false,
        unique: true
      },
      code: {
        type: DataTypes.STRING,
        allowNull: false,
        unique: true
      },
      collector: {
        type: DataTypes.BOOLEAN,
        allowNull: false,
        default: false
      },
      temporality: {
        type: DataTypes.STRING,
        allowNull: false
      },
      allow_direct_access: {
        type: DataTypes.BOOLEAN,
        allowNull: true
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
