module.exports = function(sequelize, DataTypes) {
  var DataSetOccurrence = sequelize.define("DataSetOccurrence",
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
          DataSetOccurrence.belongsTo(models.DataSet, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });

          //DataSetOccurrence.belongsTo("public.spatial_ref_sys",
          //  {foreignKey: 'fk_spatial_ref_sys_srid', targetKey: 'name'});
        }
      }
    }
  );

  return DataSetOccurrence;
};