
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
        let rows = [];
        if (tableName != "") {
          sql = `
            SELECT data_set_id
            FROM  terrama2.data_set_formats
            WHERE key = 'table_name' AND value='${tableName}';
          `;
          const result = await conn.execute(sql)
          rows = result.rows
        }

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
          dataProviderid,dataSerieTableName,isView
        } = request.query
        var key = "";

        if(isView == "true"){
          key = "view_name"
        }else{
          key = "table_name"
        }

        const conn = new Connection(URI);
        await conn.connect();
        let sql = "";
        sql = `
          SELECT data_set_id
          FROM  terrama2.data_set_formats
          WHERE key = '${key}' AND value='${dataSerieTableName}';
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

      resetAttributes: async(request, reponse) => {
        let {
          datasetId
        } = request.body
        var res = "";
        const conn = new Connection(URI);
        await conn.connect();
        var sql = `
          UPDATE terrama2.data_set_formats
          SET value = '[]'
          WHERE data_set_id = '${datasetId}' AND key = 'attributes';`;
        await conn.execute(sql);

        sql = `UPDATE terrama2.data_set_formats
          SET value = 0
          WHERE data_set_id = '${datasetId}' AND key = 'updated';`;
        await conn.execute(sql);
        await conn.disconnect();
        response.json({'status':200})
      }

    }
} ();