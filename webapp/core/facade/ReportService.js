(function() {
  'use strict';

  const DataManager = require('../../core/DataManager');
  const ViewFacade = require("../../core/facade/View");
  const {Connection} = require('../../core/utility/connection');

  const ReportService = module.exports = {};

  ReportService.findAnaliseData = async function(params) {
    const view = await ViewFacade.retrieve(viewId);
    const dataSeries = await DataManager.getDataSeries({id: view.data_series_id});
    const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id});
    const uri = dataProvider.uri;
    const filterParams = JSON.parse(filter);

    // const conn = new Connection(uri)
    const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt");
    await conn.connect();

    const dataSet = dataSeries.dataSets[0];
    let tableName = dataSet.format.table_name;

    let sqlTableName = `SELECT DISTINCT table_name FROM ${tableName}`;
    const resultTableName = await conn.execute(sqlTableName);

    tableName = resultTableName.rows[0]['table_name'];

    let sqlCount = `SELECT COUNT(*) AS count FROM public.${tableName} `;
    let sqlSelect = `SELECT *`;
    let sqlFrom = '';
    let sqlWhere = '';

    sqlSelect += `, ST_Y(ST_Transform (ST_Centroid(intersection_geom), 4326)) AS "lat",
                      ST_X(ST_Transform (ST_Centroid(intersection_geom), 4326)) AS "long"
        `;

    sqlFrom += ` FROM public.${tableName}`;

    if (date) {
      const dateFrom = date[0];
      const dateTo = date[1];

      sqlWhere += `
              WHERE execution_date::date >= '${dateFrom}' AND execution_date::date <= '${dateTo}'
          `
    }
    sqlCount += sqlWhere;

    if (area) {
      sqlWhere += ` AND calculated_area_ha > ${area}`;
    }

    if (sortColumn && sortOrder) {
      sqlWhere += ` ORDER BY ${sortColumn} ${sortOrder === '1'?'ASC':'DESC'}`;
    }

    if (limit) {
      sqlWhere += ` LIMIT ${limit}`;
    }

    if (offset) {
      sqlWhere += ` OFFSET ${offset}`;
    }

    const sql = sqlSelect + sqlFrom + sqlWhere;

    let result;
    let resultCount;
    try {
      result = await conn.execute(sql);
      let dataJson = result.rows;

      if (count) {
        resultCount = await conn.execute(sqlCount);
        dataJson.push(resultCount.rows[0]['count']);
      }

      await conn.disconnect();
      return dataJson;
    } catch (error) {
      console.log(error);
      return [];
    }
  };

} ());
