module.exports = function(sequelize, DataTypes) {
  var Alert = sequelize.define("Alert",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      active: DataTypes.BOOLEAN,
      name: DataTypes.STRING,
      description: DataTypes.STRING,
      risk_attribute: DataTypes.STRING
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          
          Alert.belongsTo(models.Project, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'project_id',
              allowNull: false
            }
          });

          Alert.belongsTo(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'data_series_id',
              allowNull: false
            }
          });

          Alert.belongsTo(models.ServiceInstance, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'instance_id',
              allowNull: false
            }
          });

          Alert.belongsTo(models.ConditionalSchedule, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'conditional_schedule_id',
              allowNull: false
            }
          });

          Alert.belongsTo(models.Risk, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'risk_id',
              allowNull: false
            }
          });

          Alert.hasOne(models.ReportMetadata, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'alert_id',
              allowNull: false
            }
          });

          Alert.hasMany(models.AlertAdditionalData, {
            onDelete: 'CASCADE',
            foreignKey: {
              allowNull: false
            }
          });

          Alert.hasMany(models.AlertNotification, {
            onDelete: 'CASCADE',
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return Alert;
};
