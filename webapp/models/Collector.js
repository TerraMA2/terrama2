module.exports = function(sequelize, DataTypes) {
  var Collector = sequelize.define("Collector",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      active: DataTypes.BOOLEAN,
      // todo: check it
      collector_type: DataTypes.INTEGER
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          Collector.hasOne(models.Filter, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false,
              unique: true
            }
          });

          Collector.hasMany(models.CollectorInputOutput, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          Collector.belongsTo(models.ServiceInstance, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          Collector.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_input",
              allowNull: false
            }
          });

          Collector.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "data_series_output",
              allowNull: false
            }
          });

          Collector.belongsTo(models.Schedule, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return Collector;
};