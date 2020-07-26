
  // Dependencies
  var handleRequestError = require("../../core/Utils").handleRequestError;
  var TokenCode = require('../../core/Enums').TokenCode;
  var Utils = require('../../core/Utils');
  var DataManager = require('../../core/DataManager');
  var {Connection} = require('../../core/utility/connection');
  const env = process.env.NODE_ENV.toLowerCase() ? process.env.NODE_ENV.toLowerCase() : 'production';
  const config = require('../../config/db')[env];


  const URI = `postgis://${config.username}:${config.password}@${config.host}:${config.port}/${config.database}`;

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
          tableName
        } = request.query

        const conn = new Connection(URI);
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
          dataSetid
        } = request.query

        const conn = new Connection(URI);
        await conn.connect();
        let rows = [];
        let sql = "";
        if (dataSetid != "") {
          sql = `
            SELECT value
            FROM terrama2.data_set_formats
            WHERE data_set_id = ${dataSetid} AND key = 'attributes';
          `;
          const result = await conn.execute(sql)
          rows = result.rows
        }
        await conn.disconnect();
        response.json(rows)
      },

      getDataSetIdByDataSerie: async (request, response) => {
        let {
          dataProviderid,dataSerieTableName
        } = request.query

        const conn = new Connection(URI);
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

        const conn = new Connection(URI);
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