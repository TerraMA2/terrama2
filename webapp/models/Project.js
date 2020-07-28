module.exports = function(sequelize, DataTypes) {
  var Project = sequelize.define("Project", {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      version: DataTypes.INTEGER,
      name: {
        type: DataTypes.STRING,
        unique: true,
        allowNull: false,
        comment: "Project name"
      },
      description: {
        type: DataTypes.TEXT,
        comment: "Project description."
      },
      protected: DataTypes.BOOLEAN,
      active: DataTypes.BOOLEAN
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      instanceMethods: {
        toObject(user = null) {
          const { protected, user_id } = this.dataValues;

          let hasPermission = !protected || (user ? (user.administrator || user.id == user_id) : false);

          return {
            class: 'Project',
            ...this.dataValues,
            hasPermission
          }
        }
      },

      classMethods: {
        associate: function(models) {
          Project.belongsTo(models.User, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "user_id",
              allowNull: true
            }
          });

          Project.hasMany(models.DataProvider, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'project_id',
              allowNull: false
            }
          });

          Project.hasMany(models.DataSeries, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'project_id',
              allowNull: false
            }
          });

          Project.hasMany(models.Legend, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'project_id',
              allowNull: false
            }
          });

          Project.hasMany(models.Analysis, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'project_id',
              allowNull: false
            }
          });
          // Setting project to View. A project has many views
          Project.hasMany(models.View, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "project_id",
              allowNull: false
            }
          });

          Project.hasMany(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "project_id",
              allowNull: false
            }
          });
        }
      }
    }
  );

  return Project;
};
