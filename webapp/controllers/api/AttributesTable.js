
  // Dependencies
  var handleRequestError = require("../../core/Utils").handleRequestError;
  var TokenCode = require('../../core/Enums').TokenCode;
  var Utils = require('../../core/Utils');
  var DataManager = require('../../core/DataManager');
  var {Connection} = require('../../core/utility/connection');

  // Facade
  var ViewFacade = require("../../core/facade/View");

  /**
   * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
   * 
   * @param {Object}
   * @returns {Object}
   */
  module.exports = function(app) {
    return {
      getDataSetId: async (request, response) => {
        let {
          tableName, viewId
        } = request.query

        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id:view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id:dataSeries.data_provider_id})
        
        const conn = new Connection(dataProvider.uri);
        await conn.connect();
        let sql = "";
        sql = `
          SELECT data_set_id
          FROM  terrama2.data_set_formats
          WHERE key = 'table_name' AND value='${tableName}';
        `;

        const result = await conn.execute(sql)
        let rows = result.rows
        await conn.disconnect();
        response.json(rows)
      },

      getAttributes: async (request, response) => {
        let {
          dataSetid,viewId
        } = request.query

        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id:view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id:dataSeries.data_provider_id})
        
        const conn = new Connection(dataProvider.uri);
        await conn.connect();
        let sql = "";
        sql = `
          SELECT value
          FROM terrama2.data_set_formats
          WHERE data_set_id = ${dataSetid} AND key = 'attributes';
        `;

        const result = await conn.execute(sql)
        let rows = result.rows
        await conn.disconnect();
        response.json(rows)
      },

      getDataSetIdByDataSerie: async (request, response) => {
        let {
          dataProviderid,dataSerieTableName
        } = request.query

        const dataProvider = await DataManager.getDataProvider({id:dataProviderid})
        
        const conn = new Connection(dataProvider.uri);
        await conn.connect();
        let sql = "";
        sql = `
          SELECT data_set_id
          FROM  terrama2.data_set_formats
          WHERE key = 'table_name' AND value='${dataSerieTableName}';
        `;

        const result = await conn.execute(sql)
        let rows = result.rows
        await conn.disconnect();
        response.json(rows)
      },

      getAttributesByDataSerie: async (request, response) => {
        let {
          dataProviderid,dataSetid
        } = request.query

        const dataProvider = await DataManager.getDataProvider({id:dataProviderid})
        
        const conn = new Connection(dataProvider.uri);
        await conn.connect();
        let sql = "";
        sql = `
          SELECT value
          FROM terrama2.data_set_formats
          WHERE data_set_id = ${dataSetid} AND key = 'attributes';
        `;

        const result = await conn.execute(sql)
        let rows = result.rows
        await conn.disconnect();
        response.json(rows)
      },
    }
} ();