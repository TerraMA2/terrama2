module.exports = function(sequelize, DataTypes) {
  var AutomaticSchedule = sequelize.define("AutomaticSchedule",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      data_ids: DataTypes.ARRAY(DataTypes.INTEGER) 
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {

          AutomaticSchedule.hasOne(models.Analysis, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'automatic_schedule_id',
              allowNull: true,
              constraints: true
            }
          });

          AutomaticSchedule.hasOne(models.View, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'automatic_schedule_id',
              allowNull: true,
              constraints: true
            }
          });
          
          AutomaticSchedule.hasOne(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'automatic_schedule_id',
              allowNull: true,
              constraints: true
            }
          });
        }
      }
    }
  );

  return AutomaticSchedule;
};
