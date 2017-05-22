module.exports = function(sequelize, DataTypes) {
  var Legend = sequelize.define("Legend",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      name: DataTypes.STRING,
      description: DataTypes.TEXT,
    },
    {
      indexes: [
        {
          unique: true,
          fields: ['project_id', 'name']
        }
      ],
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          Legend.belongsTo(models.Project, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "project_id",
              allowNull: false
            }
          });

          Legend.hasOne(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'legend_id',
              allowNull: false
            }
          });

          Legend.hasMany(models.LegendLevel, {
            onDelete: 'CASCADE',
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return Legend;
};
