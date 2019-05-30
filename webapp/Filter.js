const Sequelize = require('sequelize');
const { formatDateToTimezone } = require('./../core/Utils');

class Filter extends Sequelize.Model {
  serialize() {
    const serializedData = this.get();
    serializedData.discard_after = formatDateToTimezone(serializedData.discard_after);
    serializedData.discard_before = formatDateToTimezone(serializedData.discard_before);

    return serializedData;
  }

  static init(sequelize, DataTypes) {
    return super.init({
      frequency: DataTypes.INTEGER,
      frequency_unit: DataTypes.STRING,
      discard_before: DataTypes.DATE,
      discard_after: DataTypes.DATE,
      region: DataTypes.GEOMETRY('POLYGON'),
      by_value: DataTypes.DECIMAL,
      crop_raster: DataTypes.BOOLEAN,
      data_series_id: DataTypes.INTEGER
    },
    {
      underscored: true,
      underscoredAll: true,
      timestamps: false,
      sequelize
    })
  }

  static associate(models) {
    Filter.belongsTo(models.Collector, {
      foreignKey: {
        allowNull: false,
        unique: true
      }
    });

    Filter.belongsTo(models.DataSeries, {
      onDelete: "CASCADE",
      foreignKey: {
        name: "data_series_id",
        allowNull: true
      }
    });

    Filter.hasMany(models.ValueComparisonOperation, {
      onDelete: "CASCADE",
      foreignKey: {
        name: 'value_comparison_operation',
        allowNull: false
      }
    })
  }
}

module.exports = Filter;
