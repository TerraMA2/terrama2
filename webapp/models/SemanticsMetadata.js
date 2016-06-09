module.exports = function(sequelize, DataTypes) {
  var SemanticsMetadata = sequelize.define("SemanticsMetadata",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      key: DataTypes.STRING,
      value: DataTypes.STRING
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          SemanticsMetadata.belongsTo(models.DataSeriesSemantics, {
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

  return SemanticsMetadata;
};
