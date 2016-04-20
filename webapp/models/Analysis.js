module.exports = function(sequelize, DataTypes) {
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
      script_language: DataTypes.STRING,
      script: DataTypes.TEXT
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      freezeTableName: true,
      tableName: 'analysis',

      classMethods: {
        associate: function(models) {
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
              allowNull: false
            }
          });
        }
      }
    }
  );

  return Analysis;
};