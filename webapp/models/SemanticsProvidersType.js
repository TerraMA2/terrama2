module.exports = function(sequelize, DataTypes) {
  var SemanticsProvidersType = sequelize.define("SemanticsProvidersType",
    { },

    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      tableName: 'semantics_providers_type',

      classMethods: {
        associate: function(models) {
          SemanticsProvidersType.belongsTo(models['DataProviderType'], {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'data_provider_type_id',
              allowNull: false
            }
          });

          SemanticsProvidersType.belongsTo(models['DataSeriesSemantics'], {
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

  return SemanticsProvidersType;
};
