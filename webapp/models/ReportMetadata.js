module.exports = function(sequelize, DataTypes) {
  var ReportMetadata = sequelize.define("ReportMetadata",
    {
      id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        primaryKey: true,
        autoIncrement: true
      },
      title: DataTypes.STRING,
      abstract: DataTypes.STRING,
      description: DataTypes.TEXT,
      author: DataTypes.STRING,
      contact: DataTypes.STRING,
      copyright: DataTypes.STRING,
      timestamp_format: DataTypes.STRING,
      logo_path: DataTypes.STRING,
      document_format: DataTypes.STRING
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,

      classMethods: {
        associate: function(models) {
          ReportMetadata.belongsTo(models.Alert, {
            onDelete: "CASCADE",
            foreignKey: {
              allowNull: false
            }
          });
        }
      }
    }
  );

  return ReportMetadata;
};
