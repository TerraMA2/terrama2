module.exports = function(sequelize, DataTypes) {
  var CollectorInputOutput = sequelize.define("CollectorInputOutput",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      }
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          CollectorInputOutput.belongsTo(models.Collector, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          CollectorInputOutput.belongsTo(models.DataSet, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "input_dataset",
              allowNull: false
            }
          });

          CollectorInputOutput.belongsTo(models.DataSet, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "output_dataset",
              allowNull: false
            }
          });

          CollectorInputOutput.hasMany(models.DataCollectionLog, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return CollectorInputOutput;
};