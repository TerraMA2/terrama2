module.exports = function(sequelize, DataTypes) {
  var ValueComparisonOperation = sequelize.define("ValueComparisonOperation",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      frequency: DataTypes.INTEGER,
      frequency_unit: DataTypes.STRING,
      schedule: DataTypes.TIME,
      schedule_retry: DataTypes.INTEGER,
      schedule_retry_unit: DataTypes.STRING,
      schedule_timeout: DataTypes.INTEGER,
      schedule_timeout_unit: DataTypes.STRING
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          ValueComparisonOperation.belongsTo(models.Filter, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'value_comparison_operation',
              allowNull: false
            }
          });
        }
      }
    }
  );

  return ValueComparisonOperation;
};