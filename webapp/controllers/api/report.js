  const {Connection} = require('../../core/utility/connection');
  const DataManager = require('../../core/DataManager');
  const ViewFacade = require("../../core/facade/View");
  const ReportService = require("../../core/facade/ReportService")


  const env = process.env.NODE_ENV.toLowerCase() || 'development';
  const config = require('../../config/db')[env];


  const URI = `postgis://${config.username}:${config.password}@${config.host}:${config.port}/${config.database}`;

  /**
   * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
   *
   * @param {Object}
   * @returns {Object}
   */

  module.exports = function(app) {
    return {
      getStaticData: async (request, response) => {
        const specificParameters = JSON.parse(request.query.specificParameters);

        const conn = new Connection(URI);
        await conn.connect();

        try {
          const view = JSON.parse(specificParameters.view);

          let sqlSelect = `SELECT *`;
          let sqlFrom = '';
          let sqlWhere = '';

          sqlSelect += `, ST_Y(ST_Transform (ST_Centroid(geom), 4326)) AS "lat",
                        ST_X(ST_Transform (ST_Centroid(geom), 4326)) AS "long"
          `;


          sqlFrom += ` FROM public.${view.tableName}`;

          if (specificParameters.sortColumn && specificParameters.sortOrder) {
            sqlWhere += ` ORDER BY ${specificParameters.sortColumn} ${specificParameters.sortOrder === 1?'ASC':'DESC'}`
          }

          if (specificParameters.limit) {
            sqlWhere += ` LIMIT ${specificParameters.limit}`
          }

          if (specificParameters.offset) {
            sqlWhere += ` OFFSET ${specificParameters.offset}`
          }

          const sql = sqlSelect + sqlFrom + sqlWhere;

          let result;
          let resultCount;

          result = await conn.execute(sql);
          let dataJson = result.rows;


          let sqlCount;
          if (specificParameters.countTotal) {
            sqlCount = `SELECT COUNT(*) AS count FROM public.${view.tableName}`;
            resultCount = await conn.execute(sqlCount);

            dataJson.push(resultCount.rows[0]['count']);
          }

          response.json(dataJson)
        } catch (error) {
          console.log(error)
        } finally {
          await conn.disconnect();
        }
      },
      getDynamicData: async (request, response) => {

        const specificParameters = JSON.parse(request.query.specificParameters);
        const date = request.query.date;
        const filter = JSON.parse(request.query.filter);

        const viewParam = JSON.parse(specificParameters.view);

        const viewAuxi = await ViewFacade.retrieve(viewParam.id);
        const dataSeries = await DataManager.getDataSeries({id: viewAuxi.data_series_id});

        const conn = new Connection(URI);
        await conn.connect();

        const dataSet = dataSeries.dataSets[0];
        const tableName = dataSet.format.table_name;
        const geomColumn = dataSet.format.geometry_property;
        const timeStampColumn = dataSet.format.timestamp_property;

        let sqlSelect = `SELECT *`;
        let sqlFrom = '';
        let sqlWhere = '';

        if (geomColumn) {
          sqlSelect += `, ST_Y(ST_Transform (ST_Centroid(${geomColumn}), 4326)) AS "lat",
                        ST_X(ST_Transform (ST_Centroid(${geomColumn}), 4326)) AS "long"
          `
        }

        sqlFrom += ` FROM public.${tableName}`;

        if (date) {
          const dateFrom = date[0];
          const dateTo = date[1];

          sqlWhere += `
              WHERE ${timeStampColumn}::date >= '${dateFrom}' AND ${timeStampColumn}::date <= '${dateTo}'
          `
        }

        if (specificParameters.sortColumn && specificParameters.sortOrder) {
          sqlWhere += ` ORDER BY ${specificParameters.sortColumn} ${specificParameters.sortOrder === 1?'ASC':'DESC'}`
        }

        if (specificParameters.limit) {
          sqlWhere += ` LIMIT ${specificParameters.limit}`
        }

        if (specificParameters.offset) {
          sqlWhere += ` OFFSET ${specificParameters.offset}`
        }

        const sql = sqlSelect + sqlFrom + sqlWhere;

        let result;
        let resultCount;
        try {
          result = await conn.execute(sql);
          let dataJson = result.rows;

          let sqlCount;
          if (specificParameters.countTotal) {
            sqlCount = `SELECT COUNT(1) AS count FROM public.${tableName}`;
            resultCount = await conn.execute(sqlCount);
            dataJson.push(resultCount.rows[0]['count'])
          }

          await conn.disconnect();
          response.json(dataJson)
        } catch (error) {
          console.log(error)
        }
      },
      getAnalysisData: async (request, response) => {
        const params = JSON.parse(request.query.specificParameters);
        params.date = request.query.date;
        params.filter = request.query.filter;

        response.json(await ReportService.findAnaliseData(params));
      }
    }
} ();
