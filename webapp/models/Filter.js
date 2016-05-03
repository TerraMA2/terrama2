module.exports = function(sequelize, DataTypes) {
  var Filter = sequelize.define("Filter",
    {
      frequency: DataTypes.INTEGER,
      frequency_unit: DataTypes.STRING,
      discard_before: DataTypes.TIME,
      discard_after: DataTypes.TIME,
      region: DataTypes.GEOMETRY('POLYGON', 4326),
      by_value: DataTypes.DECIMAL
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          Filter.belongsTo(models.Collector, {
            foreignKey: {
              allowNull: false,
              unique: true
            }
          });
          
          Filter.hasMany(models.ValueComparisonOperation, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'value_comparison_operation',
              allowNull: false
            }
          })
        }
      }
    }
  );

  return Filter;
};