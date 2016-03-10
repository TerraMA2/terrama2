module.exports = function(sequelize, DataTypes) {
  var Collector = sequelize.define("Collector",
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

      classMethods: {
        associate: function(models) {
          Collector.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_input_id",
              allowNull: false
            }
          });

          Collector.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_output_id",
              allowNull: false
            }
          })
        }
      }
    }
  );

  return Collector;
};