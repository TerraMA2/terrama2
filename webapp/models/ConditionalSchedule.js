module.exports = function(sequelize, DataTypes) {
  var ConditionalSchedule = sequelize.define("ConditionalSchedule",
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

          ConditionalSchedule.hasOne(models.Analysis, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'conditional_schedule_id',
              allowNull: true,
              constraints: true
            }
          });

          ConditionalSchedule.hasOne(models.View, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'conditional_schedule_id',
              allowNull: true,
              constraints: true
            }
          });
          
          ConditionalSchedule.hasOne(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              name: 'conditional_schedule_id',
              allowNull: true,
              constraints: true
            }
          });
        }
      }
    }
  );

  return ConditionalSchedule;
};
