module.exports = function(sequelize, DataTypes) {
  var MonitorState = sequelize.define("MonitorState", {
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
    state: {
      type: DataTypes.TEXT
    }
  }, {
    underscored: true,
    underscoredAll: true,
    timestamps: false,
    classMethods: {
      associate: function(models) {
        MonitorState.belongsTo(models.User, {
          onDelete: "CASCADE",
          foreignKey: {
            name: "user_id",
            allowNull: false
          }
        });
      }
    }
  });

  return MonitorState;
};