module.exports = function(sequelize, DataTypes) {
  "use strict";

  var Analysis = sequelize.define("Analysis",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: {
        type: DataTypes.STRING,
        allowNull: false
      },
      description: DataTypes.TEXT,
      script: DataTypes.TEXT,
      active: DataTypes.BOOLEAN,
      schedule_type: DataTypes.INTEGER
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      freezeTableName: true,
      tableName: 'analysis',

      classMethods: {
        associate: function(models) {
          Analysis.hasOne(models.AnalysisOutputGrid, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'analysis_id',
              allowNull: false
            }
          });

          Analysis.hasMany(models.AnalysisMetadata, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'analysis_id',
              allowNull: false
            }
          });

          Analysis.hasMany(models.AnalysisDataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'analysis_id',
              allowNull: false
            }
          });

          Analysis.belongsTo(models.Project, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'project_id',
              allowNull: false
            }
          });

          Analysis.belongsTo(models.ServiceInstance, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'instance_id',
              allowNull: false
            }
          });

          Analysis.belongsTo(models.AnalysisType, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'type_id',
              allowNull: false
            }
          });

          Analysis.belongsTo(models.DataSet, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'dataset_output',
              allowNull: false
            }
          });

          Analysis.belongsTo(models.Schedule, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'schedule_id',
              allowNull: true
            }
          });

          Analysis.belongsTo(models.AutomaticSchedule, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'automatic_schedule_id',
              allowNull: true
            }
          });

          Analysis.belongsTo(models.ScriptLanguage, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "script_language_id",
              allowNull: false
            }
          });

          Analysis.hasOne(models.ReprocessingHistoricalData, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "analysis_id",
              allowNull: false
            }
          });
        }
      }
    }
  );

  return Analysis;
};
