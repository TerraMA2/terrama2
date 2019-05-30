const DataManager = require('./../DataManager');
const { isGeometricObjectPostGIS } = require('./../Utils');

class DataSeriesFacade {
  async save() {
    const transaction = DataManager.orm.transaction();
    const options = { transaction };

    try {
      const project = await DataManager.getProject({name: (dataSeriesObject.project ? dataSeriesObject.project : dataSeriesObject.input.project)});

      transaction.commit();
    } catch (err) {
      transaction.cancel();
    }
  }

  async internalSaveCollectorDataSeries() {

  }

  async getGeometryColumn(tableName) {
    let resultSetGeometryColumn = await DataManager.orm.query(
      `SELECT f_geometry_column as column FROM geometry_columns WHERE f_table_name = '${tableName}'`
    );

    if (resultSetGeometryColumn[0].length !== 1)
      throw new Error(`The table ${tableName} has no geometry column`);

    const geometryColumn = resultSetGeometryColumn[0][0].column;

    return geometryColumn;
  }

  async retrieveWKT(tableName, geometryColumn = null, whereCondition = '1=1') {
    if (!geometryColumn)
      geometryColumn = await this.getGeometryColumn(tableName);

    const res = await DataManager.orm.query(
      `SELECT ST_AsText(${geometryColumn}) as wkt, ST_SRID(${geometryColumn}) as srid FROM ${tableName} WHERE ${whereCondition}`
    );

    return res[0];
  }

  async retrieveWKTFromId(dataSeriesId, whereCondition = '1=1') {
    const dataSeries = await DataManager.getDataSeries({ id: dataSeriesId });

    if (!isGeometricObjectPostGIS(dataSeries))
      throw new Error(`The data series ${dataSeries.name} is not geometric object POSTGIS`);

    const { format } = dataSeries.dataSets[0];
    const tableName = format['table_name'] || format['view_name'];

    return this.retrieveWKT(tableName, null, whereCondition);
  }
}

module.exports = new DataSeriesFacade();