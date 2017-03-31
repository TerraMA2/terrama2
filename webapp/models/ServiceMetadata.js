module.exports = function(sequelize, DataTypes) {
  /**
  * It represents additional data for Service Instances
  *
  * @class ServiceMetadata
  * @type {sequelize.Model}
  */
  var ServiceMetadata = sequelize.define("ServiceMetadata",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      key: DataTypes.STRING,
      value: DataTypes.TEXT
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          ServiceMetadata.belongsTo(models.ServiceInstance, {
            onDelete: "CASCADE",
            foreignKey: {
              name: "service_instance_id",
              allowNull: false
            }
          });
        }
      }
    }
  );

  return ServiceMetadata;
};
