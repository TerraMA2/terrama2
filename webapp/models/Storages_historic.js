module.exports = function(sequelize, DataTypes) {
"use strict";

  var storage_historic = sequelize.define("storage_historic",
  {
    id: {
      type: DataTypes.INTEGER,
      allowNull: false,
      primaryKey: true,
      autoIncrement: true
    },
   process_id: {
      type: DataTypes.INTEGER,
      allowNull: false
    },
      status: {
      type: DataTypes.INTEGER,
      allowNull: false
    },
    start_timestamp: DataTypes.DATE,
    data_timestamp: DataTypes.DATE,
    last_process_timestamp: DataTypes.DATE,
    data: DataTypes.TEXT,
    origin: DataTypes.TEXT,
    type: DataTypes.INTEGER,
    description: DataTypes.TEXT
  },
  {
    underscored: true,
    underscoredAll: true,
    timestamps: false,
    classMethods: {
      associate: function(models) {
        storage_historic.belongsTo(models.Storages, {
          onDelete: "CASCADE",
          foreignKey: {
            name: "storage_id",
            allowNull: false
          }
        });
      }
    }
  });

return storage_historic;
};
